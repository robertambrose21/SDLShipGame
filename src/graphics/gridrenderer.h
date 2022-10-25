#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "textureloader.h"
#include "core/grid/grid.h"

class GridRenderer {
private:
    int windowHeight;
    int tileSize;

    std::map<int, std::shared_ptr<Texture>> tileTextures;
    std::shared_ptr<Grid> grid;

public:
    GridRenderer(std::shared_ptr<Grid> grid, int windowHeight);

    void setTileTexture(const int& tileId, std::shared_ptr<Texture> texture);
    void draw(std::shared_ptr<SDL_Renderer> renderer);

    glm::ivec2 getTilePosition(int x, int y) const;
    std::pair<int, int> getTileIndices(const glm::ivec2& position) const;
    int getTileSize(void) const;

    std::shared_ptr<Grid> getGrid(void);
};
