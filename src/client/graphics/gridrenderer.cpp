#include "gridrenderer.h"

GridRenderer::GridRenderer(Grid* grid, int windowHeight) :
    grid(grid),
    windowHeight(windowHeight),
    tileSize(32),
    textureNeedsRebuilding(true)
{
    camera = std::make_unique<Camera>(glm::ivec2(0, 0));
    grid->subscribe<TileEventData>(this);
}

void GridRenderer::setTileTexture(int tileId, uint32_t textureId) {
    game_assert(tileId >= 0);
    tileTexturesIds[tileId] = textureId;
    textureNeedsRebuilding = true;
}

void GridRenderer::buildTexture(GraphicsContext& graphicsContext) {
    auto renderer = graphicsContext.getRenderer();

    auto const& data = grid->getData();
    auto const& width = grid->getWidth();
    auto const& height = grid->getHeight();

    auto target = std::unique_ptr<SDL_Texture, Texture::sdl_deleter>(
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tileSize * width, tileSize * height), 
        Texture::sdl_deleter()
    );

    SDL_SetRenderTarget(renderer, target.get());
    SDL_RenderClear(renderer);

    for(auto y = 0; y < height; y++) {
        for(auto x = 0; x < width; x++) {
            auto const& realPosition = getTilePosition(x, y);
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
    
    texture = Texture(std::move(target));

    textureNeedsRebuilding = false;
}

void GridRenderer::draw(GraphicsContext& graphicsContext) {
    if(textureNeedsRebuilding) {
        buildTexture(graphicsContext);
    }

    auto camPos = camera->getPosition();
    SDL_Rect dst = { camPos.x, camPos.y, tileSize * grid->getWidth(), tileSize * grid->getHeight() };
    texture.draw(graphicsContext.getRenderer(), NULL, &dst);
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
    textureNeedsRebuilding = true;
}

void GridRenderer::onPublish(const Event<GridEffectEvent>& event) {
    textureNeedsRebuilding = true;
}

void GridRenderer::onPublish(const Event<GridDirtyEventData>& event) {
    if(event.data.isGridDirty) {
        textureNeedsRebuilding = true;
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