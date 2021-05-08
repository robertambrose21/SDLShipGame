#include "grid.h"

Grid::Grid(int width, int height, int windowHeight) :
    width(width),
    height(height),
    windowHeight(windowHeight),
    tileSize(windowHeight / height)
{
    data.resize(height, std::vector<std::shared_ptr<Texture>>(width));
}

void Grid::setTile(int x, int y, std::shared_ptr<Texture> tile) {
    data[y][x] = tile;
}

void Grid::draw(std::shared_ptr<SDL_Renderer> renderer) {
    for(auto y = 0; y < height; y++) {
        for(auto x = 0; x < width; x++) {
            auto position = getTilePosition(x, y);
            SDL_Rect dst = { position.x, position.y, tileSize, tileSize };
            data[y][x]->draw(renderer, NULL, &dst);
        }
    }
}

glm::ivec2 Grid::getTilePosition(int x, int y) const {
    return { x * tileSize, y * tileSize };
}

int Grid::getTileSize(void) const {
    return tileSize;
}


int Grid::getWidth(void) const {
    return width;
}

int Grid::getHeight(void) const {
    return height;
}
