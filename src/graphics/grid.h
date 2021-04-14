#pragma once

#include <vector>

#include "textureloader.h"

class Grid {
private:
    int width;
    int height;
    int windowHeight;
    int tileSize;

    std::vector<std::vector<Texture>> data;

public:
    Grid(int width, int height, int windowHeight);

    void setTile(int x, int y, Texture tile);
    void draw(std::shared_ptr<SDL_Renderer> renderer);

    SDL_Rect getTilePosition(int x, int y);
    int getTileSize(void) const;

    int getWidth(void) const;
    int getHeight(void) const;
};
