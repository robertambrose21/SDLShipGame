#include "wavefunctioncollapsestrategy.h"

bool operator<(Possibility const& lhs, Possibility const& rhs) {
    return lhs.id < rhs.id || (lhs.id == rhs.id && lhs.weight < rhs.weight);
}

bool operator==(Possibility const& lhs, Possibility const& rhs) {
    return lhs.id == rhs.id && lhs.weight == rhs.weight;
}

const int TILE_GRASS = 1;
const int TILE_GRASS_N = 0;
const int TILE_GRASS_S = 0;
const int TILE_GRASS_E = 0;
const int TILE_GRASS_W = 0;
const int TILE_WALL = 2;
const int TILE_WALL_C = 3;
const int TILE_WALL_N = 4;
const int TILE_WALL_S = 5;
const int TILE_WALL_E = 6;
const int TILE_WALL_W = 7;
const int TILE_WALL_NE = 8;
const int TILE_WALL_NW = 9;
const int TILE_WALL_SE = 10;
const int TILE_WALL_SW = 11;
const int TILE_WALL_NE2 = 20;
const int TILE_WALL_NW2 = 21;
const int TILE_WALL_SE2 = 22;
const int TILE_WALL_SW2 = 23;

enum TileEdge {
    GRASS,
    GRASS_N,
    GRASS_S,
    GRASS_E,
    GRASS_W,
    WALL,
    WALL_N,
    WALL_S,
    WALL_E,
    WALL_W
};

// NORTH, SOUTH, EAST, WEST
std::map<int, std::vector<int>> rules = {
    { TILE_GRASS,       { GRASS, GRASS, GRASS, GRASS } },
    { TILE_WALL,        { WALL, WALL, WALL, WALL } },
    { TILE_WALL_N,      { GRASS, WALL, WALL_N, WALL_N } },
    { TILE_WALL_S,      { WALL, GRASS, WALL_S, WALL_S } },
    { TILE_WALL_E,      { WALL_E, WALL_E, GRASS, WALL } },
    { TILE_WALL_W,      { WALL_W, WALL_W, WALL, GRASS } },
    { TILE_WALL_NE,     { GRASS, WALL_E, GRASS, WALL_N } },
    { TILE_WALL_NW,     { GRASS, WALL_W, WALL_N, GRASS } },
    { TILE_WALL_SE,     { WALL_E, GRASS, GRASS, WALL_S } },
    { TILE_WALL_SW,     { WALL_W, GRASS, WALL_S, GRASS } },
    { TILE_WALL_NE2,    { WALL_E, WALL, WALL_N, WALL } },
    { TILE_WALL_NW2,    { WALL_W, WALL, WALL, WALL_N } },
    { TILE_WALL_SE2,    { WALL, WALL_E, WALL_S, WALL } },
    { TILE_WALL_SW2,    { WALL, WALL_W, WALL, WALL_S } }
};

std::map<int, int> allWeights = {
    { TILE_GRASS, 4 },
    { TILE_WALL, 8 },
    { TILE_WALL_C, 1 },
    { TILE_WALL_N, 2 },
    { TILE_WALL_S, 2 },
    { TILE_WALL_E, 2 },
    { TILE_WALL_W, 2 },
    { TILE_WALL_NE, 2 },
    { TILE_WALL_NW, 2 },
    { TILE_WALL_SE, 2 },
    { TILE_WALL_SW, 2 },
    { TILE_WALL_NE2, 2 },
    { TILE_WALL_NW2, 2 },
    { TILE_WALL_SE2, 2 },
    { TILE_WALL_SW2, 2}
};

WaveFunctionCollapseStrategy::WaveFunctionCollapseStrategy(Grid* grid) :
    GenerationStrategy(grid->getWidth(), grid->getHeight()),
    grid(grid)
{
    tiles.resize(grid->getWidth(), std::vector<WFTile>(grid->getHeight()));
}

std::vector<std::vector<Grid::Tile>> WaveFunctionCollapseStrategy::generate(void) {
    for(auto y = 0; y < getHeight(); y++) {
        for(auto x = 0; x < getWidth(); x++) {
            auto& tile = tiles[y][x];

            if(x == 0 || x == getWidth() - 1 || y == 0 || y == getHeight() - 1) {
                tile.entropy = 1;
                tile.possibilities = { TILE_WALL };
            }
            else {
                tile.entropy = rules.size();
                auto kv = std::views::keys(rules);
                tile.possibilities = { kv.begin(), kv.end() };
            }

            tile.x = x;
            tile.y = y;

            addTileNeighbours(x, y, &tiles[y][x]);
        }
    }

    std::stack<WFTile*> stack;

    std::vector<glm::ivec2> points;
    for(int i = 0; i < 20; i++) {
        points.push_back(addRoom(stack));
    }

    std::sort(points.begin(), points.end());

    for(int i = 1; i < points.size(); i++) {
        auto p1 = points[i-1];
        auto p2 = points[i];
        auto intersections = grid->getIntersections(points[i-1], points[i]);

        for(auto intersection : intersections) {
            tiles[intersection.y][intersection.x].entropy = 1;
            tiles[intersection.y][intersection.x].possibilities = { TILE_GRASS };
            stack.push(&tiles[intersection.y][intersection.x]);
        }
    }

    bool collapsed = false;

    while(!collapsed) {
        collapsed = collapse(stack);
    }

    for(auto y = 0; y < getHeight(); y++) {
        for(auto x = 0; x < getWidth(); x++) {
            overrideTileId(&tiles[y][x]);

            auto tileId = tiles[y][x].possibilities[0];
            setTile(x, y, { tileId, tileId == 1, false }); // TODO: Better way of assessing whether a tile is walkable
        }
    }

    std::cout << "Done" << std::endl;

    return getData();
}

glm::ivec2 WaveFunctionCollapseStrategy::addRoom(std::stack<WFTile*>& stack) {
    int roomSizeX = randomRange(6, 15);
    int roomSizeY = randomRange(6, 15);
    int roomX = randomRange(2, getWidth() - roomSizeX - 2);
    int roomY = randomRange(2, getHeight() - roomSizeY - 2);

    for(int x = roomX; x < roomX + roomSizeX; x++) {
        for(int y = roomY; y < roomY + roomSizeY; y++) {
            tiles[y][x].entropy = 1;
            tiles[y][x].possibilities = { TILE_GRASS };
            stack.push(&tiles[y][x]);
        }
    }

    int centerX = randomRange(roomX, roomX + roomSizeX);
    int centerY = randomRange(roomY, roomY + roomSizeY);

    return glm::ivec2(centerX, centerY);
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
        connectors.push_back(rules[neighbourPossiblity][direction]);
    }

    Direction opposite;
    if(direction == NORTH) opposite = SOUTH;
    if(direction == SOUTH) opposite = NORTH;
    if(direction == EAST) opposite = WEST;
    if(direction == WEST) opposite = EAST;

    std::erase_if(tile->possibilities, [&](const auto& possiblity) {
        bool reduced = !contains(connectors, rules[possiblity][opposite]);
        return reduced;
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

bool WaveFunctionCollapseStrategy::collapse(std::stack<WFTile*>& stack) {
    auto lowestEntropyTiles = getLowestEntropyTiles();

    // std::cout << "Processing " << lowestEntropyTiles.size() << " tiles" << std::endl;

    if(lowestEntropyTiles.empty()) {
        return true;
    }

    auto tileToCollapse = randomChoice(lowestEntropyTiles);
    collapseTile(tileToCollapse);

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
    }

    return false;
}

void WaveFunctionCollapseStrategy::overrideTileId(WFTile* tile) {
    if(tile->x == 0 || tile->y == 0 || tile->x == getWidth() - 1 || tile->y == getHeight() - 1) {
        return;
    }

    if(tile->possibilities[0] == TILE_GRASS) {
        return;
    }

    auto n = tile->neighbours[NORTH]->possibilities[0];
    auto s = tile->neighbours[SOUTH]->possibilities[0];
    auto e = tile->neighbours[EAST]->possibilities[0];
    auto w = tile->neighbours[WEST]->possibilities[0];


    if(n == TILE_GRASS && s == TILE_GRASS && e == TILE_GRASS && w == TILE_GRASS) tile->possibilities[0] = TILE_WALL_C;

    else if(n != TILE_GRASS && s != TILE_GRASS && e != TILE_GRASS && w != TILE_GRASS) tile->possibilities[0] = TILE_WALL;

    else if(n == TILE_GRASS && s != TILE_GRASS && e != TILE_GRASS && w != TILE_GRASS) tile->possibilities[0] = TILE_WALL_N;
    else if(n != TILE_GRASS && s == TILE_GRASS && e != TILE_GRASS && w != TILE_GRASS) tile->possibilities[0] = TILE_WALL_S;
    else if(n != TILE_GRASS && s != TILE_GRASS && e == TILE_GRASS && w != TILE_GRASS) tile->possibilities[0] = TILE_WALL_E;
    else if(n != TILE_GRASS && s != TILE_GRASS && e != TILE_GRASS && w == TILE_GRASS) tile->possibilities[0] = TILE_WALL_W;
    
    else if(n == TILE_GRASS && s != TILE_GRASS && e != TILE_GRASS && w == TILE_GRASS) tile->possibilities[0] = TILE_WALL_NW;
    else if(n == TILE_GRASS && s != TILE_GRASS && e == TILE_GRASS && w != TILE_GRASS) tile->possibilities[0] = TILE_WALL_NE;
    else if(n != TILE_GRASS && s == TILE_GRASS && e != TILE_GRASS && w == TILE_GRASS) tile->possibilities[0] = TILE_WALL_SW;
    else if(n != TILE_GRASS && s == TILE_GRASS && e == TILE_GRASS && w != TILE_GRASS) tile->possibilities[0] = TILE_WALL_SE;
}
