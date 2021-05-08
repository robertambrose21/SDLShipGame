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

glm::ivec2 GridRenderer::getTilePosition(int x, int y) const {
    return { x * tileSize, y * tileSize };
}

int GridRenderer::getTileSize(void) const {
    return tileSize;
}
