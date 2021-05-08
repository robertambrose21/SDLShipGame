#pragma once

#include <vector>

typedef struct _tile {
    int id;
    bool isWalkable;
} Tile;

class Grid {
private:
    int width;
    int height;

    std::vector<std::vector<Tile>> data;

public:
    Grid(int width, int height, std::vector<std::vector<Tile>> data = { });

    int getWidth(void) const;
    int getHeight(void) const;

    void setTile(const int& x, const int& y, Tile tile);

    std::vector<std::vector<Tile>> getData(void) const;
    // TODO: Throw exception if x/y are out of bounds
    Tile getTileAt(const int& x, const int& y) const;
};
