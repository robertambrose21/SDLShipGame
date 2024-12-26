#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <map>

#include "spdlog/spdlog.h"

#include "core/glmimport.h"
#include "fastwfc/tiling_wfc.hpp"
#include "fastwfc/utils/array2D.hpp"
#include "fastwfc/wfc.hpp"
#include "core/util/randomutils.h"
#include "generationstrategy.h"

#include "wfctileset.h"

class WFCStrategy : public GenerationStrategy {
public:
    WFCStrategy(
        Grid* grid,
        const RoomConfiguration& roomConfiguration,
        const WFCTileSet& tileSet
    );

    std::vector<std::vector<Grid::Tile>> generate(void);

private:
    std::optional<Array2D<WFCTileSet::WFCTile>> run(
        int numAttempts, 
        int& successfulAttempt, 
        int& seed
    );
    std::optional<Array2D<WFCTileSet::WFCTile>> runAttempt(int seed);

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