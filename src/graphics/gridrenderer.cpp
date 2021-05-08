#include "gridrenderer.h"

GridRenderer::GridRenderer(int width, int height, int windowHeight) :
    width(width),
    height(height),
    windowHeight(windowHeight),
    tileSize(windowHeight / height)
{
    data.resize(height, std::vector<std::shared_ptr<Texture>>(width));
}

void GridRenderer::setTile(int x, int y, std::shared_ptr<Texture> tile) {
    data[y][x] = tile;
}

void GridRenderer::draw(std::shared_ptr<SDL_Renderer> renderer) {
    for(auto y = 0; y < height; y++) {
        for(auto x = 0; x < width; x++) {
            auto position = getTilePosition(x, y);
            SDL_Rect dst = { position.x, position.y, tileSize, tileSize };
            data[y][x]->draw(renderer, NULL, &dst);
        }
    }
}

glm::ivec2 GridRenderer::getTilePosition(int x, int y) const {
    return { x * tileSize, y * tileSize };
}

int GridRenderer::getTileSize(void) const {
    return tileSize;
}


int GridRenderer::getWidth(void) const {
    return width;
}

int GridRenderer::getHeight(void) const {
    return height;
}
