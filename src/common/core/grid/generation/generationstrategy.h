#pragma once

#include "core/grid/grid.h"

class GenerationStrategy {
private:
    std::vector<std::vector<Grid::Tile>> data;
    int width;
    int height;

public:
    GenerationStrategy(int width, int height);

    virtual std::vector<std::vector<Grid::Tile>> generate(void) = 0;

    void setTile(int x, int y, const Grid::Tile& tile);
    Grid::Tile getTile(int x, int y) const;

    int getWidth(void) const;
    int getHeight(void) const;

    const std::vector<std::vector<Grid::Tile>>& getData(void) const;
};
