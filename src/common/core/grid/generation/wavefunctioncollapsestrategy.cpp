#include "wavefunctioncollapsestrategy.h"

std::map<int, std::vector<std::vector<int>>> allPossibilities = {
    { 1, { { 1 ,2 }, { 1 ,2 }, { 1 ,2 }, { 1 ,2 } } },
    { 2, { { 1 ,2 }, { 1 ,2 }, { 1 ,2 }, { 1 ,2 } } },
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
            tile.entropy = allPossibilities.size();
            auto kv = std::views::keys(allPossibilities);
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
    auto choice = tile->possibilities[randomRange(0, tile->possibilities.size() - 1)];
    tile->possibilities = { choice };
}

bool WaveFunctionCollapseStrategy::reduceTile(WFTile* tile, const std::vector<int>& neighbourPossiblities, Direction direction) {
    if(tile->entropy <= 0) {
        return false;
    }

    bool reduced = false;

    std::vector<int> connectors;
    for(auto neighbourPossiblity : neighbourPossiblities) {
        connectors.push_back(randomChoice(allPossibilities[neighbourPossiblity][direction]));
    }

    Direction opposite;
    if(direction == NORTH) opposite = SOUTH;
    if(direction == SOUTH) opposite = NORTH;
    if(direction == EAST) opposite = WEST;
    if(direction == WEST) opposite = EAST;

    std::erase_if(tile->possibilities, [&](const auto& possiblity) {
        return !containsAny(connectors, allPossibilities[possiblity][opposite]);
    });

    tile->entropy = tile->possibilities.size();

    return reduced;
}

std::vector<WaveFunctionCollapseStrategy::WFTile*> WaveFunctionCollapseStrategy::getLowestEntropyTiles(void) {
    std::vector<WFTile*> lowestEntropyTiles;
    auto lowestEntropy = allPossibilities.size();

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

    // TODO: Random vector choice
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
        setTile(tile->x, tile->y, { tileId, tileId == 1 }); // TODO: Better way of assessing whether a tile is walkable
    }

    return false;
}
