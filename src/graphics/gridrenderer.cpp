#include "gridrenderer.h"

GridRenderer::GridRenderer(std::shared_ptr<Grid> grid, int windowHeight) :
    grid(grid),
    windowHeight(windowHeight),
    tileSize(windowHeight / grid->getHeight())
{ }

void GridRenderer::setTileTexture(const int& tileId, const uint8_t& textureId) {
    tileTextures[tileId] = textureId;
}

void GridRenderer::draw(std::shared_ptr<GraphicsContext> graphicsContext) {
    auto data = grid->getData();
    auto width = grid->getWidth();
    auto height = grid->getHeight();

    for(auto y = 0; y < height; y++) {
        for(auto x = 0; x < width; x++) {
            draw(graphicsContext, tileTextures[data[y][x].id], { x, y });
        }
    }
}

void GridRenderer::draw(std::shared_ptr<GraphicsContext> graphicsContext, const uint8_t& textureId, const glm::ivec2& position) {
    game_assert(graphicsContext != nullptr);
    auto realPosition = getTilePosition(position.x, position.y);
    SDL_Rect dst = { realPosition.x, realPosition.y, getTileSize(), getTileSize() };
    graphicsContext->getTextureLoader()->loadTexture(textureId)->draw(graphicsContext->getRenderer(), NULL, &dst);
}

glm::ivec2 GridRenderer::getTilePosition(int x, int y) const {
    return { x * tileSize, y * tileSize };
}

std::pair<int, int> GridRenderer::getTileIndices(const glm::ivec2& position) const {
    return { position.x / tileSize, position.y / tileSize };
}

int GridRenderer::getTileSize(void) const {
    return tileSize;
}

std::shared_ptr<Grid> GridRenderer::getGrid(void) {
    return grid;
}