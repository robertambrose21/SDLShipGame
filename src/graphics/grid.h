#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "textureloader.h"

class Grid {
private:
    int width;
    int height;
    int windowHeight;
    int tileSize;

    std::vector<std::vector<std::shared_ptr<Texture>>> data;

public:
    Grid(int width, int height, int windowHeight);

    void setTile(int x, int y, std::shared_ptr<Texture> tile);
    void draw(std::shared_ptr<SDL_Renderer> renderer);

    glm::ivec2 getTilePosition(int x, int y) const;
    int getTileSize(void) const;

    int getWidth(void) const;
    int getHeight(void) const;
};
