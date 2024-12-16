#include "wfc2.h"

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