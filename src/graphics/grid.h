#pragma once

#include <vector>

#include "textureloader.h"

class Grid {
private:
    int width;
    int height;
    int windowHeight;

    std::vector<std::vector<Texture>> data;

public:
    Grid(int width, int height, int windowHeight);

    void setTile(int x, int y, Texture tile);
    void draw(std::shared_ptr<SDL_Renderer> renderer);
};
