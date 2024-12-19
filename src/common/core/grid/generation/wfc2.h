#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <map>

#include "core/glmimport.h"
#include "fastwfc/tiling_wfc.hpp"
#include "fastwfc/overlapping_wfc.hpp"
#include "fastwfc/utils/array2D.hpp"
#include "fastwfc/wfc.hpp"
#include "core/util/randomutils.h"
#include "generationstrategy.h"
#include "core/json.hpp"

using json = nlohmann::json;

class WFC2 : public GenerationStrategy {
private:
    typedef struct _wfcTile {
        unsigned index; // TODO: check if this can be a uint8_t
        Symmetry symmetry;
        std::string name;
        double weight;
        int textureId;
        uint8_t orientation;
    } WFCTile;

    std::string rulesFile;

    Symmetry getSymmetry(char symmetry);

public:
    WFC2(
        Grid* grid,
        const RoomConfiguration& roomConfiguration,
        const std::string& rulesFile
    );

    std::vector<std::vector<Grid::Tile>> generate(void);

    void run(void);
};