#include "generationstrategy.h"

GenerationStrategy::GenerationStrategy(int width, int height) :
    width(width),
    height(height)
{
    data.resize(height, std::vector<Grid::Tile>(width));
}

void GenerationStrategy::setTile(int x, int y, const Grid::Tile& tile) {
    game_assert(x < getWidth() && y < getHeight());
    data[y][x] = tile;
}

Grid::Tile GenerationStrategy::getTile(int x, int y) const {
    game_assert(x >= 0 && y >= 0);
    game_assert(x < getWidth() && y < getHeight());
    return data[y][x];
}

int GenerationStrategy::getWidth(void) const {
    return width;
}

int GenerationStrategy::getHeight(void) const {
    return height;
}

const std::vector<std::vector<Grid::Tile>>& GenerationStrategy::getData(void) const {
    return data;
}