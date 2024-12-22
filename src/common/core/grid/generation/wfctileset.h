#pragma once

#include <string>
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>

#include "fastwfc/tiling_wfc.hpp"
#include "fastwfc/utils/array2D.hpp"
#include "fastwfc/wfc.hpp"
#include "core/json.hpp"

using json = nlohmann::json;

class WFCTileSet {
public:
    typedef struct _wfcTile {
        uint8_t id;
        Symmetry symmetry;
        std::string name;
        double weight;
        int textureId;
        uint8_t orientation;
    } WFCTile;

    WFCTileSet(const std::string& rulesFile);

    const std::vector<Tile<WFCTile>>& getTiles(void) const;
    const std::vector<std::tuple<unsigned, unsigned, unsigned, unsigned>>& getNeighbours(void) const;
    const std::map<std::string, WFCTile>& getTileMapping(void) const;

    const std::map<unsigned, bool>& getWalkableTiles(void) const;
    bool isTileWalkable(unsigned id);

    unsigned getEdgeTile(void) const;
    unsigned getRoomTile(void) const;

    void load(void);
    void reset(void);

private:
    bool validate(void) const;
    Symmetry getSymmetry(char symmetry);

    bool isError;
    bool isLoaded;
    std::string rulesFile;

    std::vector<Tile<WFCTile>> tiles;
    std::vector<std::tuple<unsigned, unsigned, unsigned, unsigned>> neighbours;
    std::map<unsigned, bool> walkableTiles;
    std::map<std::string, WFCTile> tileMapping;
    unsigned edgeTile;
    unsigned roomTile;
};