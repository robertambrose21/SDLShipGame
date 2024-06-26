#include "gridrenderer.h"

GridRenderer::GridRenderer(Grid* grid, int windowHeight) :
    grid(grid),
    windowHeight(windowHeight),
    tileSize(32)
{
    camera = std::make_unique<Camera>(glm::ivec2(0, 0));
    grid->subscribe<TileEventData>(this);
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

    for(int x = 0; x < chunksX; x++) {
        for(int y = 0; y < chunksY; y++) {
            int xMin = x * ChunkSize;
            int xMax = std::min(grid->getWidth() - 1, ((x + 1) * ChunkSize) - 1);
            int yMin = y * ChunkSize;
            int yMax = std::min(grid->getHeight() - 1, ((y + 1) * ChunkSize) - 1);

            Chunk chunk;
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

    auto const& data = grid->getData();

    auto target = std::unique_ptr<SDL_Texture, Texture::sdl_deleter>(
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tileSize * ChunkSize, tileSize * ChunkSize), 
        Texture::sdl_deleter()
    );

    SDL_SetRenderTarget(renderer, target.get());
    SDL_RenderClear(renderer);

    for(auto y = chunk->min.y; y <= chunk->max.y; y++) {
        for(auto x = chunk->min.x; x <= chunk->max.x; x++) {
            auto const& realPosition = getTilePosition(x - chunk->min.x, y - chunk->min.y);
            SDL_Rect dst = { realPosition.x, realPosition.y, getTileSize(), getTileSize() };
            graphicsContext.getTextureLoader().loadTexture(tileTexturesIds[data[y][x].id])->draw(renderer, NULL, &dst);

            if(grid->getTileAt(x, y).isFrozen) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0x7F);
                SDL_RenderFillRect(renderer, &dst);
            }
        }
    }

    SDL_SetRenderTarget(renderer, NULL);
    
    chunk->texture = std::make_unique<Texture>(Texture(std::move(target)));
    chunk->textureNeedsRebuilding = false;
}

bool GridRenderer::isTileInChunk(Chunk* chunk, int x, int y) {
    return x >= chunk->min.x && x <= chunk->max.x && y >= chunk->min.y && y <= chunk->max.y;
}

void GridRenderer::draw(GraphicsContext& graphicsContext) {
    for(auto& chunk : chunks) {
        if(chunk->textureNeedsRebuilding) {
            buildChunkTexture(graphicsContext, chunk.get());    
        }

        auto camPos = camera->getPosition();
        SDL_Rect dst = { camPos.x + (chunk->min.x * tileSize), camPos.y + (chunk->min.y * tileSize), tileSize * ChunkSize, tileSize * ChunkSize };
        chunk->texture->draw(graphicsContext.getRenderer(), NULL, &dst);
    }
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

Camera& GridRenderer::getCamera(void) {
    return *camera;
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