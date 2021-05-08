#include "grid.h"

Grid::Grid(int width, int height, std::vector<std::vector<Tile>> data) :
    width(width),
    height(height),
    data(data)
{
    if(data.empty()) {
        this->data.resize(height, std::vector<Tile>(width));
    }
}

int Grid::getWidth(void) const {
    return width;
}

int Grid::getHeight(void) const {
    return height;
}

void Grid::setTile(const int& x, const int& y, Tile tile) {
    data[y][x] = tile;
}

std::vector<std::vector<Tile>> Grid::getData(void) const {
    return data;
}

Tile Grid::getTileAt(const int& x, const int& y) const {
    return data[y][x];
}