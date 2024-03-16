#pragma once

#include <map>
#include <stack>
#include <ranges>

#include "generationstrategy.h"
#include "core/util/randomrolls.h"

class WaveFunctionCollapseStrategy : public GenerationStrategy {
private:
    enum Direction {
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

    typedef struct _wfTile {
        int x, y;
        int entropy; // This could maybe just be driven off possibilities.size()?
        std::vector<int> possibilities;
        std::map<Direction, _wfTile*> neighbours;
    } WFTile;

    std::vector<std::vector<WFTile>> tiles;

    void addTileNeighbours(int x, int y, WFTile* tile);
    void collapseTile(WFTile* tile);
    bool reduceTile(WFTile* tile, const std::vector<int>& possibilities, Direction direction);

    std::vector<WFTile*> getLowestEntropyTiles(void);
    bool collapse(void);

public:
    WaveFunctionCollapseStrategy(int width, int height);

    std::vector<std::vector<Grid::Tile>> generate(void);
};