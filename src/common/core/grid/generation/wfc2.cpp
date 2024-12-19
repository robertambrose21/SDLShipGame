#include "wfc2.h"

WFC2::WFC2(
    Grid* grid,
    const RoomConfiguration& roomConfiguration,
    const std::string& rulesFile
) : 
    GenerationStrategy(grid->getWidth(), grid->getHeight(), roomConfiguration),
    rulesFile(rulesFile)
{ }

std::vector<std::vector<Grid::Tile>> WFC2::generate(void) {
    std::cout << "Generating map... " << std::endl;

    if(!std::filesystem::exists(rulesFile)) {
        std::cout << "Cannot load rules, path \"" << rulesFile << "\" does not exist";
        return getData();
    }

    std::ifstream f(rulesFile);
    json data = json::parse(f);

    auto tilesJson = data["tiles"].get<std::vector<json>>();
    std::map<std::string, WFCTile> tileMapping;
    unsigned index = 0;

    for(auto const& tile : tilesJson) {
        auto name = tile["name"].get<std::string>();

        if(tileMapping.contains(name)) {
            std::cout << std::format("Duplicate tile '{}' found in rules {}. Cannot generate", name, rulesFile) << std::endl;
            return getData();
        }

        tileMapping[name] = {
            index++,
            getSymmetry(tile["symmetry"].get<std::string>()[0]),
            name, 
            tile["weight"].get<double>(),
            tile["textureId"].get<int>()
        };
    }

    auto neighboursJson = data["neighbours"].get<std::vector<json>>();
    std::vector<std::tuple<unsigned, unsigned, unsigned, unsigned>> neighbours;

    for(auto const& neighbour : neighboursJson) {
        neighbours.push_back({ 
            tileMapping[neighbour["left"].get<std::string>()].index,
            neighbour["left_orientation"].get<unsigned>(),
            tileMapping[neighbour["right"].get<std::string>()].index,
            neighbour["right_orientation"].get<unsigned>()
        });
    }

    auto walkableTiles = data["walkableTiles"].get<std::set<std::string>>();

    std::vector<Tile<WFCTile>> wfcTiles;

    for(auto const& [name, tile] : tileMapping) {
        switch(tile.symmetry) {
            // No symmetry
            case Symmetry::X:
                wfcTiles.push_back({ { Array2D<WFCTile>(1, 1, tile) }, tile.symmetry, tile.weight });
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

                wfcTiles.push_back({ data, tile.symmetry, tile.weight });
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
                return getData();
        }
    }

    auto seed = 1825110143;//randomRange(0, INT_MAX);
    TilingWFC<WFCTile> wfc(wfcTiles, neighbours, getHeight(), getWidth(), { false }, seed);

    auto success = wfc.run();

    if(!success.has_value()) {
        std::cout << "Failed generating map" << std::endl;
        return getData();
    }

    for(int x = 0; x < getWidth(); x++) {
        for(int y = 0; y < getHeight(); y++) {
            auto const& wfcTile = (*success).data[y * getWidth() + x];
            
            setTile(x, y, { 
                wfcTile.textureId, 
                walkableTiles.contains(wfcTile.name), 
                false, 
                wfcTile.orientation 
            });

            std::cout << wfcTile.textureId << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "Done, seed: " << seed << std::endl;
    return getData();
}

Symmetry WFC2::getSymmetry(char symmetry) {
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

void WFC2::run(void) {
    // unsigned width = 32;
    // unsigned height = 32;

    // OverlappingWFCOptions options = {
    //   false, 
    //   true, 
    //   width, 
    //   height, 
    //   8, 
    //   false, 
    //   2
    // };

    // // Array2D<unsigned char> input(3, 3);
    // // input.data = {
    // //     0, 1, 0,
    // //     1, 0, 1,
    // //     0, 1, 0
    // // };

    // Array2D<uint8_t> input(8, 7);
    // input.data = {
    //     0, 0, 0, 0, 0, 0, 0,
    //     0, 1, 2, 2, 2, 3, 0,
    //     0, 4, 9, 9, 9, 5, 0,
    //     0, 4, 9, 9, 9, 5, 0,
    //     0, 4, 9, 9, 9, 5, 0,
    //     0, 6, 7, 7, 7, 8, 0,
    //     0, 0, 0, 0, 0, 0, 0,
    //     0, 0, 0, 0, 0, 0, 0
    // };

    // OverlappingWFC<uint8_t> wfc(input, options, 1);
    // auto success = wfc.run();

    // if(!success.has_value()) {
    //     std::cout << "Woopsie" << std::endl;
    //     return;
    // }

    // auto output = *success;

    // for(int x = 0; x < width; x++) {
    //     for(int y = 0; y < height; y++) {
    //         std::cout << (int) output.data[y * width + x] << " ";
    //     }
    //     std::cout << std::endl;
    // }

    unsigned width = 32;
    unsigned height = 32;

    // std::vector<Tile<uint8_t>> tiles = {
    //     { { Array2D<uint8_t>(1, 1, 0) }, Symmetry::X, 1 },     // Grass
    //     { { Array2D<uint8_t>(1, 1, 1) }, Symmetry::X, 1 },      // Dirt
    //     // { { Array2D<uint8_t>(1, 1, 2) }, Symmetry::T, 1 },      // Dirt edge
    //     // { { Array2D<uint8_t>(1, 1, 3) }, Symmetry::L, 1 },      // Dirt corner
    //     { { Array2D<uint8_t>(1, 1, 4) }, Symmetry::X, 1 },     // Wall
    //     // { { Array2D<uint8_t>(1, 1, 5) }, Symmetry::T, 2 },      // Wall edge
    //     // { { Array2D<uint8_t>(1, 1, 6) }, Symmetry::L, 2 },      // Wall corner
    // };

    std::vector<Tile<std::string>> tiles = {
        { { Array2D<std::string>(1, 1, "_") }, Symmetry::X, 1 },     // Grass
        { { Array2D<std::string>(1, 1, "*") }, Symmetry::X, 1 },      // Dirt
        // { { Array2D<uint8_t>(1, 1, 2) }, Symmetry::T, 1 },      // Dirt edge
        // { { Array2D<uint8_t>(1, 1, 3) }, Symmetry::L, 1 },      // Dirt corner
        { { Array2D<std::string>(1, 1, "X") }, Symmetry::X, 2 },     // Wall
        { { 
                Array2D<std::string>(1, 1, "╗"),
                Array2D<std::string>(1, 1, "╝"),
                Array2D<std::string>(1, 1, "╚"),
                Array2D<std::string>(1, 1, "╔"),
            }, Symmetry::L, 1 
        },                                                      // Wall corner
        { { 
                Array2D<std::string>(1, 1, "╠"),
                Array2D<std::string>(1, 1, "╦"),
                Array2D<std::string>(1, 1, "╣"),
                Array2D<std::string>(1, 1, "╩"),
            }, Symmetry::T, 2
        },                                                      // Wall edge
        { { 
                Array2D<std::string>(1, 1, "╗"),
                Array2D<std::string>(1, 1, "╝"),
                Array2D<std::string>(1, 1, "╚"),
                Array2D<std::string>(1, 1, "╔"),
            }, Symmetry::L, 1 
        },                                                      // Wall corner 2
    };

    std::vector<std::tuple<unsigned, unsigned, unsigned, unsigned>> neighbours = {
        { 0, 0, 0, 0 },
        { 1, 0, 1, 0 },
        { 0, 0, 1, 0 },
        { 2, 0, 2, 0 },

        // Corners
        { 3, 0, 4, 2 },
        { 1, 0, 3, 0 },

        { 3, 1, 1, 0 },
        { 4, 2, 3, 1 },

        { 3, 2, 1, 0 },
        { 4, 0, 3, 2 },

        { 3, 3, 4, 0 },
        { 1, 0, 3, 3 },

        // Corners 2
        { 2, 0, 5, 0 },
        { 5, 0, 4, 0 },

        { 4, 0, 5, 1 },
        { 5, 1, 2, 0 },

        { 4, 2, 5, 2 },
        { 5, 2, 2, 0 },

        { 2, 0, 5, 3 },
        { 5, 3, 4, 2 },

        // Edges
        { 4, 0, 4, 0 },
        { 4, 0, 3, 2 },
        { 3, 3, 4, 0 },

        { 1, 0, 4, 1 },
        { 4, 1, 2, 0 },
        
        { 4, 2, 4, 2 },
        { 4, 2, 3, 1 },
        { 3, 0, 4, 2 },

        { 2, 0, 4, 3 },
        { 4, 3, 1, 0 },
    };

    TilingWFC<std::string> wfc(tiles, neighbours, height, width, { false }, randomRange(0, INT_MAX));

    auto success = wfc.run();

    if(!success.has_value()) {
        std::cout << "Woopsie" << std::endl;
        return;
    }

    auto output = *success;

    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height; y++) {
            std::cout << output.data[y * width + x] << " ";
        }
        std::cout << std::endl;
    }
}