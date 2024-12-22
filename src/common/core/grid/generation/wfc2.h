#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <map>

#include "core/glmimport.h"
#include "fastwfc/tiling_wfc.hpp"
#include "fastwfc/utils/array2D.hpp"
#include "fastwfc/wfc.hpp"
#include "core/util/randomutils.h"
#include "generationstrategy.h"

#include "wfctileset.h"

class WFC2 : public GenerationStrategy {
public:
    WFC2(
        Grid* grid,
        const RoomConfiguration& roomConfiguration,
        const WFCTileSet& tileSet
    );

    std::vector<std::vector<Grid::Tile>> generate(void);

    void run(void);

private:
    void generateMapEdge(TilingWFC<WFCTileSet::WFCTile>& wfc);
    void generateRoomsAndPaths(TilingWFC<WFCTileSet::WFCTile>& wfc);
    Room generateRoom(
        TilingWFC<WFCTileSet::WFCTile>& wfc, 
        const std::vector<Room>& existingRooms
    );
    Room createRandomRoom(void);

    Grid* grid;
    WFCTileSet tileSet;
};