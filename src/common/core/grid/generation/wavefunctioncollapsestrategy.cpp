#include "wavefunctioncollapsestrategy.h"

bool operator<(Possibility const& lhs, Possibility const& rhs) {
    return lhs.id < rhs.id || (lhs.id == rhs.id && lhs.weight < rhs.weight);
}

bool operator==(Possibility const& lhs, Possibility const& rhs) {
    return lhs.id == rhs.id && lhs.weight == rhs.weight;
}

// std::map<int, int> allWeights = {
//     { 1, 8 },
//     { 2, 1 }
// };

// std::map<int, std::vector<std::vector<int>>> allPossibilities = {
//     { 1, { { 1, 2 }, { 1, 2 }, { 1, 2 }, { 1, 2 } } },
//     { 2, { { 1, 2 }, { 1, 2 }, { 1, 2 }, { 1, 2 } } },
// };

std::map<int, int> allWeights = {
    { 1, 16 },
    { 2, 8 },
    // { 3, 1 },
    { 4, 4 },
    { 5, 4 },
    { 6, 4 },
    { 7, 4 },
    { 8, 2 },
    { 9, 2 },
    { 10, 2 },
    { 11, 2 },
    // { 12, 1 },
    // { 13, 1 },
    // { 14, 1 },
    // { 15, 1 }
    { 16, 4 },
    { 17, 4 },
    { 18, 4 },
    { 19, 4 }
};

// NORTH, SOUTH, EAST, WEST
// std::map<int, std::vector<std::vector<int>>> rules = {
//     { 1, { { 1 }, { 1 }, { 1 }, { 1 } } },      // GRASS
//     { 2, { { 2 }, { 2 }, { 2 }, { 2 } } },      // WALL
//     // { 3, { { 1 }, { 1 }, { 1 }, { 1 } } },      // WALL_C
//     { 4, { { 1 }, { 2 }, { 4 }, { 4 } } },      // WALL_N
//     { 5, { { 2 }, { 1 }, { 5 }, { 5 } } },      // WALL_S
//     { 6, { { 6 }, { 6 }, { 1 }, { 2 } } },      // WALL_E
//     { 7, { { 7 }, { 7 }, { 2 }, { 1 } } },      // WALL_W
//     { 8, { { 1 }, { 6 }, { 1 }, { 4 } } },      // WALL_NE
//     { 9, { { 1 }, { 7 }, { 4 }, { 1 } } },      // WALL_NW
//     { 10, { { 6 }, { 1 }, { 1 }, { 5 } } },     // WALL_SE
//     { 11, { { 7 }, { 1 }, { 5 }, { 1 } } }//,     // WALL_SW
//     // { 12, { { 1 }, { 2 }, { 1 }, { 1 } } },     // WALL_NC
//     // { 13, { { 2 }, { 1 }, { 1 }, { 1 } } },     // WALL_SC
//     // { 14, { { 1 }, { 1 }, { 1 }, { 2 } } },     // WALL_EC
// };

// This is just fucked lol - enum it all otherwise it's just fucking hell
// NORTH, SOUTH, EAST, WEST
std::map<int, std::vector<std::vector<int>>> rules = {
    { 1, { { 1 }, { 1 }, { 1 }, { 1 } } },      // GRASS
    { 2, { { 2 }, { 2 }, { 2 }, { 2 } } },      // WALL
    // { 3, { { 1 }, { 1 }, { 1 }, { 1 } } },      // WALL_C
    { 4, { { 16 }, { 2 }, { 4 }, { 4 } } },      // WALL_N
    { 5, { { 2 }, { 17 }, { 5 }, { 5 } } },      // WALL_S
    { 6, { { 6 }, { 6 }, { 18 }, { 2 } } },      // WALL_E
    { 7, { { 7 }, { 7 }, { 2 }, { 19 } } },      // WALL_W
    { 8, { { 16 }, { 6 }, { 16 }, { 4 } } },     // WALL_NE
    { 9, { { 17 }, { 7 }, { 4 }, { 17 } } },     // WALL_NW
    { 10, { { 6 }, { 18 }, { 18 }, { 5 } } },    // WALL_SE
    { 11, { { 7 }, { 19 }, { 5 }, { 19 } } },    // WALL_SW
    // { 12, { { 1 }, { 2 }, { 1 }, { 1 } } },     // WALL_NC
    // { 13, { { 2 }, { 1 }, { 1 }, { 1 } } },     // WALL_SC
    // { 14, { { 1 }, { 1 }, { 1 }, { 2 } } },     // WALL_EC
    // { 15, { { 1 }, { 1 }, { 2 }, { 1 } } },     // WALL_WC
    { 16, { { 1 }, { 5 }, { 16 }, { 16 } } },    // GRASS_N
    { 17, { { 4 }, { 1 }, { 17 }, { 17 } } },    // GRASS_S
    { 18, { { 18 }, { 18 }, { 1 }, { 7 } } },    // GRASS_E
    { 19, { { 19 }, { 19 }, { 6 }, { 1 } } },    // GRASS_W
    { 16, { { 1 }, { 18 }, { 1 }, { 16 } } },    // GRASS_NE
    { 17, { { 1 }, { 16 }, { 19 }, { 1 } } },    // GRASS_NW
    { 18, { { 18 }, { 1 }, { 1 }, { 17 } } },    // GRASS_SE
    { 19, { { 19 }, { 1 }, { 17 }, { 1 } } }     // GRASS_SW
};


WaveFunctionCollapseStrategy::WaveFunctionCollapseStrategy(int width, int height) :
    GenerationStrategy(width, height)
{
    tiles.resize(height, std::vector<WFTile>(width));

    for(auto y = 0; y < getHeight(); y++) {
        for(auto x = 0; x < getWidth(); x++) {
            auto& tile = tiles[y][x];

            tile.x = x;
            tile.y = y;
            tile.entropy = rules.size();
            auto kv = std::views::keys(rules);
            tile.possibilities = { kv.begin(), kv.end() };

            addTileNeighbours(x, y, &tiles[y][x]);
        }
    }
}

std::vector<std::vector<Grid::Tile>> WaveFunctionCollapseStrategy::generate(void) {
    bool collapsed = false;

    while(!collapsed) {
        collapsed = collapse();
    }

    return getData();
}

void WaveFunctionCollapseStrategy::addTileNeighbours(int x, int y, WFTile* tile) {
    if(y > 0) {
        tile->neighbours[NORTH] = &tiles[y - 1][x];
    }
    if(y < getHeight() - 1) {
        tile->neighbours[SOUTH] = &tiles[y + 1][x];
    }
    if(x < getWidth() - 1) {
        tile->neighbours[EAST] = &tiles[y][x + 1];
    }
    if(x > 0) {
        tile->neighbours[WEST] = &tiles[y][x - 1];
    }
}

void WaveFunctionCollapseStrategy::collapseTile(WFTile* tile) {
    tile->entropy = 0;

    std::vector<int> weights;
    for(auto possiblity : tile->possibilities) {
        weights.push_back(allWeights[possiblity]);
    }

    tile->possibilities = { randomChoice(tile->possibilities, weights) };
}

bool WaveFunctionCollapseStrategy::reduceTile(
    WFTile* tile, 
    const std::vector<int>& neighbourPossiblities, 
    Direction direction
) {
    if(tile->entropy <= 0) {
        return false;  
    }

    bool reduced = false;

    std::vector<int> connectors;
    for(auto neighbourPossiblity : neighbourPossiblities) {
        for(auto possibility : rules[neighbourPossiblity][direction]) {
            connectors.push_back(possibility);
        }
    }

    Direction opposite;
    if(direction == NORTH) opposite = SOUTH;
    if(direction == SOUTH) opposite = NORTH;
    if(direction == EAST) opposite = WEST;
    if(direction == WEST) opposite = EAST;

    // std::erase_if(tile->possibilities, [&](const auto& possiblity) {
    //     return !containsAny(connectors, rules[possiblity][opposite]);
    // });
    std::erase_if(tile->possibilities, [&](const auto& possiblity) {
        return !containsAny(connectors, rules[possiblity][opposite]);
    });

    tile->entropy = tile->possibilities.size();

    return reduced;
}

std::vector<WaveFunctionCollapseStrategy::WFTile*> WaveFunctionCollapseStrategy::getLowestEntropyTiles(void) {
    std::vector<WFTile*> lowestEntropyTiles;
    auto lowestEntropy = rules.size();

    for(auto y = 0; y < getHeight(); y++) {
        for(auto x = 0; x < getWidth(); x++) {
            auto entropy = tiles[y][x].entropy;

            if(entropy <= 0) {
                continue;
            }

            if(entropy < lowestEntropy) {
                lowestEntropyTiles.clear();
                lowestEntropy = entropy;
            }

            if(entropy == lowestEntropy) {
                lowestEntropyTiles.push_back(&tiles[y][x]);
            }
        }
    }

    return lowestEntropyTiles;
}

bool WaveFunctionCollapseStrategy::collapse(void) {
    auto lowestEntropyTiles = getLowestEntropyTiles();

    std::cout << "Processing " << lowestEntropyTiles.size() << " tiles" << std::endl;

    if(lowestEntropyTiles.empty()) {
        return true;
    }

    auto tileToCollapse = randomChoice(lowestEntropyTiles);
    collapseTile(tileToCollapse);

    std::stack<WFTile*> stack;
    stack.push(tileToCollapse);

    while(!stack.empty()) {
        auto tile = stack.top();
        auto tilePossibilities = tile->possibilities;
        stack.pop();

        for(auto& [direction, neighbour] : tile->neighbours) {
            if(neighbour->entropy != 0) {
                if(reduceTile(neighbour, tilePossibilities, direction)) {
                    stack.push(neighbour);
                }
            }
        }

        auto tileId = tile->possibilities[0];
        if(tileId == 0) {
            std::cout << "Literally not possible lol" << std::endl;
        }
        setTile(tile->x, tile->y, { tileId, tileId == 1, false }); // TODO: Better way of assessing whether a tile is walkable
    }

    return false;
}
