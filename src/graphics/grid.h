#pragma once

#include <vector>

#include "tile.h"

class Grid {
private:
    int width;
    int height;
    int windowHeight;

    std::vector<std::vector<Tile>> data;

public:
    Grid(int width, int height, int windowHeight);

    void setTile(int x, int y, Tile tile);
    void draw(std::shared_ptr<SDL_Renderer> renderer);
};
