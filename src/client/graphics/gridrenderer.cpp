#include "gridrenderer.h"

GridRenderer::GridRenderer(
    Grid* grid, 
    VisiblityController* visiblityController,
    EntityPool* entityPool,
    int windowHeight
) :
    grid(grid),
    visiblityController(visiblityController),
    entityPool(entityPool),
    windowHeight(windowHeight),
    tileSize(32),
    participant(nullptr),
    fogTextureNeedsRebuilding(true)
{
    grid->subscribe<TileEventData>(this);
    grid->subscribe<GridDirtyEventData>(this);
    visiblityController->subscribe<TilesRevealedEventData>(this);
    entityPool->subscribe<EntitySetPositionEventData>(this);

    camera = std::make_unique<Camera>(glm::ivec2(0, 0));
    chunks = createChunks();
}

void GridRenderer::setTileTexture(int tileId, uint32_t textureId) {
    game_assert(tileId >= 0);
    tileTexturesIds[tileId] = textureId;
}

std::vector<std::unique_ptr<GridRenderer::Chunk>> GridRenderer::createChunks(void) {
    std::vector<std::unique_ptr<Chunk>> chunks;

    int chunksX = grid->getWidth() / ChunkSize;
    int chunksY = grid->getHeight() / ChunkSize;

    int lastChunkX = grid->getWidth() % ChunkSize;
    int lastChunkY = grid->getHeight() % ChunkSize;

    if(lastChunkX > 0) chunksX++;
    if(lastChunkY > 0) chunksY++;

    int chunkId = 0;

    for(int x = 0; x < chunksX; x++) {
        for(int y = 0; y < chunksY; y++) {
            int xMin = x * ChunkSize;
            int xMax = std::min(grid->getWidth() - 1, ((x + 1) * ChunkSize) - 1);
            int yMin = y * ChunkSize;
            int yMax = std::min(grid->getHeight() - 1, ((y + 1) * ChunkSize) - 1);

            Chunk chunk;
            chunk.id = chunkId++;
            chunk.min = glm::ivec2(xMin, yMin);
            chunk.max = glm::ivec2(xMax, yMax);
            chunk.texture = nullptr;
            chunk.textureNeedsRebuilding = true;

            chunks.push_back(std::make_unique<Chunk>(std::move(chunk)));
        }
    }

    return chunks;
}

void GridRenderer::buildChunkTexture(GraphicsContext& graphicsContext, Chunk* chunk) {
    auto renderer = graphicsContext.getRenderer();

    // auto const& data = grid->getData();
    auto const& data = visiblityController->getRevealedTiles(1);

    auto target = std::unique_ptr<SDL_Texture, Texture::sdl_deleter>(
        SDL_CreateTexture(
            renderer, 
            SDL_PIXELFORMAT_RGBA8888, 
            SDL_TEXTUREACCESS_TARGET, 
            tileSize * ChunkSize, 
            tileSize * ChunkSize
        ), 
        Texture::sdl_deleter()
    );

    SDL_SetRenderTarget(renderer, target.get());
    SDL_RenderClear(renderer);

    for(auto& revealedTile : data) {
        auto [x, y, isVisible, tile] = revealedTile;

        if(x < chunk->min.x || x > chunk->max.x || y < chunk->min.y || y > chunk->max.y) {
            continue;
        }

        auto colour = isVisible ? Texture::Colour { 0xFF, 0xFF, 0xFF } : Texture::Colour { 0x7F, 0x7F, 0x7F };

        auto const& realPosition = getTilePosition(x - chunk->min.x, y - chunk->min.y);
        SDL_Rect dst = { realPosition.x, realPosition.y, getTileSize(), getTileSize() };
        graphicsContext.getTextureLoader().loadTexture(tileTexturesIds[tile.id])
            ->draw(renderer, colour, 0xFF, NULL, &dst);

        if(grid->getTileAt(x, y).isFrozen) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0x7F);
            SDL_RenderFillRect(renderer, &dst);
        }
    }

    // for(auto y = chunk->min.y; y <= chunk->max.y; y++) {
    //     for(auto x = chunk->min.x; x <= chunk->max.x; x++) {
    //         auto const& realPosition = getTilePosition(x - chunk->min.x, y - chunk->min.y);
    //         SDL_Rect dst = { realPosition.x, realPosition.y, getTileSize(), getTileSize() };
    //         graphicsContext.getTextureLoader().loadTexture(tileTexturesIds[data[y][x].id])
    //             ->draw(renderer, NULL, &dst);

    //         if(grid->getTileAt(x, y).isFrozen) {
    //             SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    //             SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0x7F);
    //             SDL_RenderFillRect(renderer, &dst);
    //         }
    //     }
    // }

    SDL_SetRenderTarget(renderer, NULL);
    
    chunk->texture = std::make_unique<Texture>(Texture(std::move(target)));
    chunk->textureNeedsRebuilding = false;
}

void GridRenderer::buildFogBorders(GraphicsContext& graphicsContext, int xMin, int xMax, int yMin, int yMax) {
    auto renderer = graphicsContext.getRenderer();

    SDL_Rect left =     { 0, 0, xMin * tileSize, grid->getHeight() * tileSize };
    SDL_Rect right =    { xMax * tileSize, 0, (grid->getWidth() - xMax) * tileSize, grid->getHeight() * tileSize };
    SDL_Rect top =      { xMin * tileSize, 0, (xMax - xMin) * tileSize, yMin * tileSize };
    SDL_Rect bottom =   { xMin * tileSize, yMax * tileSize, (xMax - xMin) * tileSize, (grid->getHeight() - yMax) * tileSize };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);
    SDL_RenderFillRect(renderer, &top);    
    SDL_RenderFillRect(renderer, &bottom);
}

void GridRenderer::buildFogTiles(
    GraphicsContext& graphicsContext, 
    Entity* entity, 
    int xMin, 
    int xMax, 
    int yMin, 
    int yMax
) {
    auto renderer = graphicsContext.getRenderer();

    auto tiles = grid->getVisibleTiles(
        glm::vec2(entity->getPosition().x, entity->getPosition().y),
        entity->getAggroRange()
    );

    std::unordered_set<glm::ivec2, glm::ivec2Hash> visibleTiles(tiles.begin(), tiles.end());

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

    for(int x = xMin; x < xMax; x++) {
        for(int y = yMin; y < yMax; y++) {
            if(visibleTiles.find(glm::ivec2(x, y)) == visibleTiles.end()) {
                auto const& realPosition = getTilePosition(x, y);
                SDL_Rect dst = { realPosition.x, realPosition.y, getTileSize(), getTileSize() };

                SDL_RenderFillRect(renderer, &dst);
            }
        }
    }
}

void GridRenderer::buildFogTexture(GraphicsContext& graphicsContext) {
    auto startTime = getCurrentTimeInMicroseconds();

    auto renderer = graphicsContext.getRenderer();

    auto target = std::unique_ptr<SDL_Texture, Texture::sdl_deleter>(
        SDL_CreateTexture(
            renderer, 
            SDL_PIXELFORMAT_RGBA8888, 
            SDL_TEXTUREACCESS_TARGET, 
            tileSize * grid->getWidth(),
            tileSize * grid->getHeight()
        ), 
        Texture::sdl_deleter()
    );

    SDL_SetRenderTarget(renderer, target.get());
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);

    if(participant != nullptr) {
        for(auto const& entity : participant->getEntities()) {
            int xMin = std::max(entity->getPosition().x - entity->getAggroRange() - 1, 0);
            int xMax = std::min(entity->getPosition().x + entity->getAggroRange() + 1, grid->getWidth());
            int yMin = std::max(entity->getPosition().y - entity->getAggroRange() - 1, 0);
            int yMax = std::min(entity->getPosition().y + entity->getAggroRange() + 1, grid->getWidth());

            buildFogBorders(graphicsContext, xMin, xMax, yMin, yMax);
            buildFogTiles(graphicsContext, entity, xMin, xMax, yMin, yMax);
        }
    }

    SDL_SetRenderTarget(renderer, NULL);
    
    fogTexture = std::make_unique<Texture>(Texture(std::move(target)));
    fogTextureNeedsRebuilding = false;

    auto timeTaken = getCurrentTimeInMicroseconds() - startTime;

    std::cout << std::format("buildFogTexture: {}ms", (double) timeTaken / 1000.0) << std::endl;
}

// TODO: Chunk this
void GridRenderer::buildDebugTexture(GraphicsContext& graphicsContext) {
    auto renderer = graphicsContext.getRenderer();

    auto target = std::unique_ptr<SDL_Texture, Texture::sdl_deleter>(
        SDL_CreateTexture(
            renderer, 
            SDL_PIXELFORMAT_RGBA8888, 
            SDL_TEXTUREACCESS_TARGET, 
            tileSize * grid->getWidth(),
            tileSize * grid->getHeight()
        ), 
        Texture::sdl_deleter()
    );

    SDL_SetRenderTarget(renderer, target.get());
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);

    for(int x = 0; x < grid->getWidth(); x++) {
        auto p1 = glm::ivec2(x * tileSize, 0);
        auto p2 = glm::ivec2(x * tileSize, grid->getHeight() * tileSize);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);
        SDL_RenderDrawLine(graphicsContext.getRenderer(), p1.x, p1.y, p2.x, p2.y);

        for(int y = 0; y < grid->getHeight(); y++) {
            auto const& realPosition = getTilePosition(x, y);

            auto font = Font::loadFromFile("../assets/fonts/RobotoMono-SemiBold.ttf", 10);
            auto text = Text(
                std::move(font), 
                realPosition,
                glm::ivec2(tileSize, tileSize), 
                { 0xFF, 0xFF, 0x00, 0xFF }
            );
            text.draw(renderer, std::format("{}\n{}", x, y));
        }
    }

    for(int y = 0; y < grid->getHeight(); y++) {
        auto p1 = glm::ivec2(0, y * tileSize);
        auto p2 = glm::ivec2(grid->getWidth() * tileSize, y * tileSize);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);
        SDL_RenderDrawLine(graphicsContext.getRenderer(), p1.x, p1.y, p2.x, p2.y);
    }

    SDL_SetRenderTarget(renderer, NULL);

    debugTexture = std::make_unique<Texture>(Texture(std::move(target)));
}

void GridRenderer::drawDebugTexture(GraphicsContext& graphicsContext) {
    if(debugTexture == nullptr) {
        buildDebugTexture(graphicsContext);
    }

    auto camPos = camera->getPosition();

    SDL_Rect debugDst = { camPos.x, camPos.y, tileSize * grid->getWidth(), tileSize * grid->getHeight() };
    debugTexture->draw(graphicsContext.getRenderer(), { 0xFF, 0xFF, 0xFF }, 0x7F, NULL, &debugDst);
}

bool GridRenderer::isTileInChunk(Chunk* chunk, int x, int y) {
    return x >= chunk->min.x && x <= chunk->max.x && y >= chunk->min.y && y <= chunk->max.y;
}

void GridRenderer::draw(GraphicsContext& graphicsContext) {
    auto camPos = camera->getPosition();

    for(auto& chunk : chunks) {
        if(chunk->textureNeedsRebuilding) {
            buildChunkTexture(graphicsContext, chunk.get());    
        }

        SDL_Rect dst = { 
            camPos.x + (chunk->min.x * tileSize), 
            camPos.y + (chunk->min.y * tileSize), 
            tileSize * ChunkSize, 
            tileSize * ChunkSize 
        };
        chunk->texture->draw(graphicsContext.getRenderer(), NULL, &dst);
    }

    // TODO: rebuild on entity set position
    if(fogTextureNeedsRebuilding) {
        buildFogTexture(graphicsContext);
    }

    SDL_Rect fogDst = { camPos.x, camPos.y, 4096, 4096 };
    fogTexture->draw(graphicsContext.getRenderer(), { 0xFF, 0xFF, 0xFF }, 0x7F, NULL, &fogDst);

    // drawDebugTexture(graphicsContext);
}

void GridRenderer::draw(
    GraphicsContext& graphicsContext, 
    uint32_t textureId,
    const glm::ivec2& position
) {
    auto renderer = graphicsContext.getRenderer();

    auto const& realPosition = getTilePosition(position.x, position.y) + camera->getPosition();
    SDL_Rect dst = { realPosition.x, realPosition.y, getTileSize(), getTileSize() };
    graphicsContext.getTextureLoader().loadTexture(textureId)->draw(renderer, NULL, &dst);

    if(grid->getTileAt(position.x, position.y).isFrozen) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0x7F);
        SDL_RenderFillRect(renderer, &dst);
    }
}

void GridRenderer::draw(
    GraphicsContext& graphicsContext, 
    uint32_t textureId,
    const Texture::Colour& colour,
    const uint8_t alpha,
    const glm::ivec2& position
) {
    auto const& realPosition = getTilePosition(position.x, position.y) + camera->getPosition();
    SDL_Rect dst = { realPosition.x, realPosition.y, getTileSize(), getTileSize() };
    graphicsContext.getTextureLoader().loadTexture(textureId)->draw(
        graphicsContext.getRenderer(), colour, alpha, NULL, &dst
    );
}

void GridRenderer::onPublish(const Event<TileEventData>& event) {
    for(auto& chunk : chunks) {
        chunk->textureNeedsRebuilding = chunk->textureNeedsRebuilding || isTileInChunk(chunk.get(), event.data.x, event.data.y);
    }
}

void GridRenderer::onPublish(const Event<GridEffectEvent>& event) {
    for(auto& chunk : chunks) {
        chunk->textureNeedsRebuilding = true;
    }
}

void GridRenderer::onPublish(const Event<GridDirtyEventData>& event) {
    if(!event.data.isGridDirty) {
        return;
    }

    for(auto& chunk : chunks) {
        chunk->textureNeedsRebuilding = true;
    }
}

void GridRenderer::onPublish(const Event<TilesRevealedEventData>& event) {
    // TODO: Handle properly
    if(event.data.participantId != 1) {
        return;
    }

    for(auto& tile : event.data.tiles) {
        for(auto& chunk : chunks) {
            chunk->textureNeedsRebuilding = chunk->textureNeedsRebuilding || isTileInChunk(chunk.get(), tile.x, tile.y);
        }
    }

    fogTextureNeedsRebuilding = true;
}

void GridRenderer::onPublish(const Event<EntitySetPositionEventData>& event) {
    fogTextureNeedsRebuilding = true;
}

Camera& GridRenderer::getCamera(void) {
    return *camera;
}

void GridRenderer::setParticipant(Participant* participant) {
    game_assert(participant != nullptr);
    game_assert(participant->getIsPlayer());
    this->participant = participant;
}

Participant* GridRenderer::getParticipant(void) {
    return participant;
}

glm::ivec2 GridRenderer::getTilePosition(int x, int y) const {
    return { x * tileSize, y * tileSize };
}

std::pair<int, int> GridRenderer::getTileIndices(const glm::ivec2& position) {
    return { position.x / tileSize, position.y / tileSize };
}

int GridRenderer::getTileSize(void) const {
    return tileSize;
}

Grid* GridRenderer::getGrid(void) {
    return grid;
}