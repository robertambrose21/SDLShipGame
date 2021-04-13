#include "grid.h"

Grid::Grid(int width, int height, int windowHeight) :
    width(width),
    height(height),
    windowHeight(windowHeight)
{
    data.resize(height, std::vector<Texture>(width));
}

void Grid::setTile(int x, int y, Texture tile) {
    data[y][x] = tile;
}

void Grid::draw(std::shared_ptr<SDL_Renderer> renderer) {
    const int size = windowHeight / height;

    for(auto y = 0; y < height; y++) {
        for(auto x = 0; x < width; x++) {
            SDL_Rect src = { 0, 0, size, size };
            SDL_Rect dst = { x * size, y * size, size, size };
            data[y][x].draw(renderer, &src, &dst);
        }
    }
}