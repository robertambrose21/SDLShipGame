#include "tileset.h"

TileSet::TileSet()
{ }

TileSet::TileSet(const std::string& path) {
    load(path);
}

void TileSet::load(const std::string& path) {
    std::ifstream f(path); // TODO: Check exists
    json data = json::parse(f);

    int numEdges = 0;
    std::map<std::string, int> edges;

    for(auto const& edgeData : data["edges"].get<std::vector<std::string>>()) {
        edges[edgeData] = numEdges++;
    }

    int numTiles = 0;
    std::map<std::string, int> tileNames;

    for(auto const& typesData : data["types"].get<std::vector<json>>()) {
        auto key = typesData.items().begin().key();

        // TODO: error check variant
        int variant = 0;
        for(auto const& typeData : typesData[key].get<json>()) {
            TileSet::Tile tile;
            tile.name = typeData["tile"].get<std::string>();
            tile.type = key;
            tile.weight = typeData["weight"].get<int>();

            tiles[numTiles] = tile;
            variants[key][(TileSet::Variant) variant++] = numTiles;
            textureIds[numTiles] = typeData["textureId"].get<int>();
            tileNames[tile.name] = numTiles++;
        }
    }

    for(auto const& ruleData : data["rules"].get<std::vector<json>>()) {
        auto key = ruleData.items().begin().key();

        auto ruleEdges = ruleData[key].get<std::vector<std::string>>();

        for(auto const& ruleEdge : ruleEdges) {
            rules[tileNames[key]].push_back(edges[ruleEdge]);
        }
    }
}

const std::map<int, uint32_t>& TileSet::getTextureIds(void) const {
    return textureIds;
}

const std::map<std::string, std::map<TileSet::Variant, int>>& TileSet::getVariants(void) const {
    return variants;
}

const std::map<int, TileSet::Tile>& TileSet::getTiles(void) const {
    return tiles;
}

const std::map<int, std::vector<int>>& TileSet::getRules(void) {
    return rules;
}

int TileSet::getNumVariantsForType(const std::string& type) {
    return variants[type].size();
}

const int TileSet::getTileVariantIdForType(const std::string& type, TileSet::Variant variant) {
    return variants[type][variant];
}

const TileSet::Tile TileSet::getTile(int id) {
    return tiles[id];
}

const int TileSet::getRule(int neighbourId, int direction) {
    return rules[neighbourId][direction];
}