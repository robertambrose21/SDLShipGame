#pragma once

#include <map>
#include <stack>
#include <ranges>
#include <fstream>

#include "core/json.hpp"
#include "generationstrategy.h"
#include "core/util/randomutils.h"
#include "core/util/vectorutils.h"
#include "tileset.h"

using json = nlohmann::json;

class WaveFunctionCollapseStrategy : public GenerationStrategy {
private:
    enum Direction {
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

    typedef struct _wfChunk {
        int xMin, xMax;
        int yMin, yMax;
    } WFChunk;

    typedef struct _wfTile {
        int x, y;
        int entropy;
        bool seeded;
        std::vector<int> possibilities;
        std::map<Direction, _wfTile*> neighbours;
    } WFTile;

    Grid* grid;
    std::vector<std::vector<WFTile>> tiles;
    std::vector<std::vector<WFTile>> tilesSnapshot;
    std::stack<WFTile*> tilesToCollapse;
    TileSet tileSet;
    int tilesCollapsed;

    bool isCollapsed;

    void addTileNeighbours(int x, int y, WFTile* tile);
    void collapseTile(WFTile* tile);
    bool reduceTile(WFTile* tile, const std::vector<int>& possibilities, Direction direction);

    std::vector<WFTile*> getLowestEntropyTiles(const WFChunk& chunk);
    bool collapse(const WFChunk& chunk);

    std::vector<WFChunk> generateChunks(int chunkSize);
    void generateWallsAndNeighbours(void);
    void generateRoomsAndPaths(void);
    bool subgenerate(int chunkSize, int numRetries, WFChunk& chunk, int numChunks);
    
    Room generateRoom(const std::vector<Room>& existingRooms);
    Room createRandomRoom(void);
    bool hasCollision(const Room& roomA, const Room& roomB);

    void overrideTiles(void);
    void overrideTileId(WFTile* tile);
    bool overrideCornerTileId(WFTile* tile, const std::string& type);
    bool overrideEdgeTileId(WFTile* tile, const std::string& type);


public:
    WaveFunctionCollapseStrategy(
        Grid* grid,
        const TileSet& tileSet,
        const RoomConfiguration& roomConfiguration
    );

    std::vector<std::vector<Grid::Tile>> generate(void);
};
