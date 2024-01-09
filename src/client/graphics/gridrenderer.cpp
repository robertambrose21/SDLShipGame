#include "gridrenderer.h"

GridRenderer::GridRenderer(Grid* grid, int windowHeight) :
    grid(grid),
    windowHeight(windowHeight),
    tileSize(32)
{
    camera = std::make_unique<Camera>(glm::ivec2(0, 0));
}

void GridRenderer::setTileTexture(int tileId, uint32_t textureId) {
    game_assert(tileId >= 0);
    tileTextures[tileId] = textureId;
}

void GridRenderer::draw(GraphicsContext& graphicsContext) {
    auto const& data = grid->getData();
    auto const& width = grid->getWidth();
    auto const& height = grid->getHeight();

    for(auto y = 0; y < height; y++) {
        for(auto x = 0; x < width; x++) {
            // if(data[y][x].isWalkable) {
                draw(graphicsContext, tileTextures[data[y][x].id], { x, y });
            // }
        }
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

    if(grid->getTileAt(position.x, position.y).turnsFrozenFor > 0) {
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