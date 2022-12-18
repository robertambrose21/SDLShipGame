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

    std::map<int, uint32_t> tileTextures;
    std::shared_ptr<Grid> grid;

public:
    GridRenderer(const std::shared_ptr<Grid>& grid, int windowHeight);

    void setTileTexture(int tileId, uint32_t textureId);
    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);
    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext, uint32_t textureId, const glm::ivec2& position);
    void draw(
        const std::shared_ptr<GraphicsContext>& graphicsContext,
        uint32_t textureId,
        const Texture::Colour& colour,
        const uint8_t alpha,
        const glm::ivec2& position
    );

    glm::ivec2 getTilePosition(int x, int y) const;
    std::pair<int, int> getTileIndices(const glm::ivec2& position) const;
    int getTileSize(void) const;

    std::shared_ptr<Grid> getGrid(void);
};
