#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "graphicscontext.h"
#include "textureloader.h"
#include "core/grid/grid.h"

class GraphicsContext;

class GridRenderer {
private:
    int windowHeight;
    int tileSize;

    std::map<int, uint8_t> tileTextures;
    std::shared_ptr<Grid> grid;

public:
    GridRenderer(const std::shared_ptr<Grid>& grid, const int& windowHeight);

    void setTileTexture(const int& tileId, const uint8_t& textureId);
    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);
    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext, const uint8_t& textureId, const glm::ivec2& position);

    glm::ivec2 getTilePosition(const int& x, const int& y) const;
    std::pair<int, int> getTileIndices(const glm::ivec2& position) const;
    int getTileSize(void) const;

    std::shared_ptr<Grid> getGrid(void);
};
