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

    typedef struct _roomConfiguration {
        int numRooms;
        glm::ivec2 minRoomSize;
        glm::ivec2 maxRoomSize;
    } RoomConfiguration;

    Grid* grid;
    std::vector<std::vector<WFTile>> tiles;
    std::stack<WFTile*> tilesToCollapse;
    RoomConfiguration roomConfiguration;

    bool isCollapsed;

    void addTileNeighbours(int x, int y, WFTile* tile);
    void collapseTile(WFTile* tile);
    bool reduceTile(WFTile* tile, const std::vector<int>& possibilities, Direction direction);

    std::vector<WFTile*> getLowestEntropyTiles(void);
    bool collapse(void);

    void generateWallsAndNeighbours(void);
    void generateRoomsAndPaths(void);
    
    glm::ivec2 addRoom(void);

    void overrideTiles(void);
    void overrideTileId(WFTile* tile);

public:
    WaveFunctionCollapseStrategy(Grid* grid, const RoomConfiguration& roomConfiguration);

    std::vector<std::vector<Grid::Tile>> generate(void);
};