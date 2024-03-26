#pragma once

#include <map>
#include <stack>
#include <ranges>

#include "generationstrategy.h"
#include "core/util/randomutils.h"
#include "core/util/vectorutils.h"

typedef struct _possibility {
    int id;
    int weight = 1;
} Possibility;

bool operator<(Possibility const& lhs, Possibility const& rhs);
bool operator==(Possibility const& lhs, Possibility const& rhs);

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

    Grid* grid;

    std::vector<std::vector<WFTile>> tiles;

    void addTileNeighbours(int x, int y, WFTile* tile);
    void collapseTile(WFTile* tile);
    bool reduceTile(WFTile* tile, const std::vector<int>& possibilities, Direction direction);

    std::vector<WFTile*> getLowestEntropyTiles(void);
    bool collapse(std::stack<WFTile*>& stack);

    glm::ivec2 addRoom(std::stack<WFTile*>& stack);

    void overrideTileId(WFTile* tile);

public:
    WaveFunctionCollapseStrategy(Grid* grid);

    std::vector<std::vector<Grid::Tile>> generate(void);
};