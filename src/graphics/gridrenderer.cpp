#include "gridrenderer.h"

GridRenderer::GridRenderer(std::shared_ptr<Grid> grid, int windowHeight) :
    grid(grid),
    windowHeight(windowHeight),
    tileSize(windowHeight / grid->getHeight())
{ }

void GridRenderer::setTileTexture(const int& tileId, std::shared_ptr<Texture> texture) {
    tileTextures[tileId] = texture;
}

void GridRenderer::draw(std::shared_ptr<SDL_Renderer> renderer) {
    auto data = grid->getData();
    auto width = grid->getWidth();
    auto height = grid->getHeight();

    for(auto y = 0; y < height; y++) {
        for(auto x = 0; x < width; x++) {
            auto position = getTilePosition(x, y);
            SDL_Rect dst = { position.x, position.y, tileSize, tileSize };

            tileTextures[data[y][x].id]->draw(renderer, NULL, &dst);
        }
    }
}

void GridRenderer::draw(std::shared_ptr<GraphicsContext> graphicsContext, const uint8_t& textureId, const glm::ivec2& position) {
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