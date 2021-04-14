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
            SDL_Rect dst = getTilePosition(x, y);
            data[y][x]->draw(renderer, NULL, &dst);
        }
    }
}

SDL_Rect Grid::getTilePosition(int x, int y) {
    return { x * tileSize, y * tileSize, tileSize, tileSize };
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
