#pragma once

#include <map>
#include <stack>
#include <ranges>
#include <fstream>

#include "core/json.hpp"
#include "generationstrategy.h"
#include "core/util/randomutils.h"
#include "core/util/vectorutils.h"

using json = nlohmann::json;

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
        int entropy;
        std::vector<int> possibilities;
        std::map<Direction, _wfTile*> neighbours;
    } WFTile;

    typedef struct _roomConfiguration {
        int numRooms;
        glm::ivec2 minRoomSize;
        glm::ivec2 maxRoomSize;
    } RoomConfiguration;

    // typedef struct _tileSet {
    //     typedef struct _type {
    //         std::string type;
    //         int textureId;
    //         int weight;
    //     } Type;

    //     // std::vector<int> edges;
    //     std::map<int, Type> types;
    //     std::map<int, std::vector<int>> rules;
    // } TileSet;

    typedef struct _tileSet {
        enum Variant {
            MIDDLE, 
            CENTER,
            NORTH,
            SOUTH,
            EAST,
            WEST,
            NORTH_EAST,
            NORTH_WEST,
            SOUTH_EAST,
            SOUTH_WEST,
            NORTH_EAST_2,
            NORTH_WEST_2,
            SOUTH_EAST_2,
            SOUTH_WEST_2
        };

        typedef struct _type {
            std::string tile;
            std::string type;
            Variant variant;
            int textureId;
            int weight;
        } Type;

        std::map<int, Type> types;
        std::map<int, std::vector<int>> rules;
    } TileSet;

    Grid* grid;
    std::vector<std::vector<WFTile>> tiles;
    std::stack<WFTile*> tilesToCollapse;
    RoomConfiguration roomConfiguration;
    TileSet tileSet;

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

    void loadTileSet(const std::string& path);

public:
    WaveFunctionCollapseStrategy(Grid* grid, const RoomConfiguration& roomConfiguration);

    std::vector<std::vector<Grid::Tile>> generate(void);
};