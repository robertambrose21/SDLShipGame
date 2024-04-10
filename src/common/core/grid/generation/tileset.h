#pragma once

#include <map>
#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "core/json.hpp"

using json = nlohmann::json;

class TileSet {
public:
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
        SOUTH_WEST_2,
        COUNT
    };

    typedef struct _tile {
        std::string name;
        std::string type;
        int weight;
    } Tile;

private:
    std::map<int, uint32_t> textureIds;
    std::map<std::string, std::map<Variant, int>> variants;
    std::map<int, Tile> tiles;
    std::map<int, std::vector<int>> rules;

    void load(const std::string& path);

public:
    TileSet();
    TileSet(const std::string& path);

    const std::map<int, uint32_t>& getTextureIds(void) const;
    const std::map<std::string, std::map<Variant, int>>& getVariants(void) const;
    const std::map<int, Tile>& getTiles(void) const;
    const std::map<int, std::vector<int>>& getRules(void);

    int getNumVariantsForType(const std::string& type);
    const int getTileVariantIdForType(const std::string& type, Variant variant);
    const Tile getTile(int id);
    const int getRule(int neighbourId, int direction);
};
