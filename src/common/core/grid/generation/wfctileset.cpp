#include "wfctileset.h"

WFCTileSet::WFCTileSet(const std::string& rulesFile) :
    rulesFile(rulesFile),
    isLoaded(false),
    isError(false),
    edgeTile(0),
    roomTile(0)
{ }

void WFCTileSet::load(void) {
    if(isError) {
        std::cout 
            << std::format("Error on loading previous tileset '{}', please reset before attempting to load again", rulesFile) 
            << std::endl;
        return;
    }

    if(isLoaded) {
        std::cout 
            << std::format("Already loaded tileset '{}', please reset before attempting to load again", rulesFile)
            << std::endl;
        return;
    }

    // Assume an error state until we've finished loading
    isError = true;

    if(!std::filesystem::exists(rulesFile)) {
        std::cout << std::format("Cannot load rules, path '{}' does not exist", rulesFile) << std::endl;
        return;
    }

    std::ifstream f(rulesFile);
    json data = json::parse(f);

    auto tilesJson = data["tiles"].get<std::vector<json>>();

    for(auto const& tile : tilesJson) {
        auto name = tile["name"].get<std::string>();

        if(tileMapping.contains(name)) {
            std::cout << std::format("Duplicate tile '{}' found in rules {}. Cannot generate", name, rulesFile) << std::endl;
            return;
        }

        tileMapping[name] = {
            tile["id"].get<uint8_t>(),
            getSymmetry(tile["symmetry"].get<std::string>()[0]),
            name,
            tile["weight"].get<double>(),
            tile["textureId"].get<int>()
        };
    }

    std::map<std::string, unsigned> tileIndexMapping; 
    unsigned index = 0;
    for(auto [name, tile] : tileMapping) {
        tileIndexMapping[name] = index++;
    }

    auto neighboursJson = data["neighbours"].get<std::vector<json>>();

    for(auto const& neighbour : neighboursJson) {
        neighbours.push_back({ 
            tileIndexMapping[neighbour["left"].get<std::string>()],
            neighbour["left_orientation"].get<unsigned>(),
            tileIndexMapping[neighbour["right"].get<std::string>()],
            neighbour["right_orientation"].get<unsigned>()
        });
    }

    auto walkableTilesJson = data["walkableTiles"].get<std::set<unsigned>>();

    for(auto const& [name, tile] : tileMapping) {
        walkableTiles[tile.id] = walkableTilesJson.contains(tile.id);

        switch(tile.symmetry) {
            // No symmetry
            case Symmetry::X:
                tiles.push_back({ { Array2D<WFCTile>(1, 1, tile) }, tile.symmetry, tile.weight });
                break;

            // Rotational symmetry
            case Symmetry::T:
            case Symmetry::L: {
                std::vector<Array2D<WFCTile>> data;
                
                // 0 -> 4 becomes 0 -> 270 rotational degrees
                for(int i = 0; i < 4; i++) {
                    auto variant = tile;
                    variant.orientation = i;
                    data.push_back(Array2D<WFCTile>(1, 1, variant));
                }

                tiles.push_back({ data, tile.symmetry, tile.weight });
                break;
            }

            // TODO:
            case Symmetry::I:
            case Symmetry::backslash:
            case Symmetry::P:
                std::cout << "Symmetry I, \\ and P are currently unsupported" << std::endl;
                break;

            // Exit if we somehow get a weird symmetry
            default:
                std::cout << std::format("Unknown symmetry '{}'", (int) tile.symmetry) << std::endl;
                return;
        }
    }

    edgeTile = tileIndexMapping[data["edgeTile"].get<std::string>()];
    roomTile = tileIndexMapping[data["rooms"]["roomTile"].get<std::string>()];

    isError = false;
    isLoaded = true;
}

void WFCTileSet::reset(void) {
    tiles.clear();
    neighbours.clear();
    walkableTiles.clear();
    tileMapping.clear();
    isLoaded = false;
    isError = false;
}

Symmetry WFCTileSet::getSymmetry(char symmetry) {
    switch(symmetry) {
        case 'X': return Symmetry::X;
        case 'T': return Symmetry::T;
        case 'I': return Symmetry::I;
        case 'L': return Symmetry::L;
        case '\\': return Symmetry::backslash;
        case 'P': return Symmetry::P;
        default: {
            std::cout << std::format("Cannot parse invalid symmetry '{}', defaulting to 'X'", symmetry);
            return Symmetry::X;
        }
    }
}

bool WFCTileSet::validate(void) const {
    if(!isLoaded || isError) {
        std::cout << std::format("Warning: Loaded {}, Error {}", isLoaded, isError) << std::endl;
        return false;
    }

    return true;
}

const std::vector<Tile<WFCTileSet::WFCTile>>& WFCTileSet::getTiles(void) const {
    validate();
    return tiles;
}

const std::vector<std::tuple<unsigned, unsigned, unsigned, unsigned>>& WFCTileSet::getNeighbours(void) const {
    validate();
    return neighbours;
}

const std::map<unsigned, bool>& WFCTileSet::getWalkableTiles(void) const {
    validate();
    return walkableTiles;
}

bool WFCTileSet::isTileWalkable(unsigned id) {
    if(!validate()) {
        return false;
    }

    return walkableTiles.contains(id) && walkableTiles[id];
}

const std::map<std::string, WFCTileSet::WFCTile>& WFCTileSet::getTileMapping(void) const {
    validate();
    return tileMapping;
}

unsigned WFCTileSet::getEdgeTile(void) const {
    validate();
    return edgeTile;
}

unsigned WFCTileSet::getRoomTile(void) const {
    validate();
    return roomTile;
}
