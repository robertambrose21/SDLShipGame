#include "wavefunctioncollapsestrategy.h"

WaveFunctionCollapseStrategy::WaveFunctionCollapseStrategy(Grid* grid, const RoomConfiguration& roomConfiguration) :
    GenerationStrategy(grid->getWidth(), grid->getHeight()),
    grid(grid),
    roomConfiguration(roomConfiguration),
    isCollapsed(false)
{
    loadTileSet("../assets/data/tilesets/grass_and_rocks/rules.json");
}

void WaveFunctionCollapseStrategy::loadTileSet(const std::string& path) {
    std::ifstream f(path); // TODO: Check exists
    json data = json::parse(f);

    int numEdges = 0;
    std::map<std::string, int> edges;

    for(auto const& edgeData : data["edges"].get<std::vector<std::string>>()) {
        edges[edgeData] = numEdges++;
    }

    int numTiles = 0;
    std::map<std::string, int> tiles;

    for(auto const& typesData : data["types"].get<std::vector<json>>()) {
        auto key = typesData.items().begin().key();

        // TODO: error check variant
        int variant = 0;
        for(auto const& typeData : typesData[key].get<json>()) {
            TileSet::Type type;
            type.tile = typeData["tile"].get<std::string>();
            type.type = key;
            type.textureId = typeData["textureId"].get<int>();
            type.weight = typeData["weight"].get<int>();

            tileSet.types[numTiles] = type;
            tileSet.variants[key][(TileSet::Variant) variant++] = numTiles;
            tiles[type.tile] = numTiles++;
        }
    }

    for(auto const& ruleData : data["rules"].get<std::vector<json>>()) {
        auto key = ruleData.items().begin().key();

        auto ruleEdges = ruleData[key].get<std::vector<std::string>>();

        for(auto const& ruleEdge : ruleEdges) {
            tileSet.rules[tiles[key]].push_back(edges[ruleEdge]);
        }
    }
}

std::vector<std::vector<Grid::Tile>> WaveFunctionCollapseStrategy::generate(void) {
    bool isCollapsed = false;
    tiles.resize(grid->getWidth(), std::vector<WFTile>(grid->getHeight()));
    tilesToCollapse = {};
    
    generateWallsAndNeighbours();
    generateRoomsAndPaths();

    while(!isCollapsed) {
        isCollapsed = collapse();
    }

    overrideTiles();

    std::cout << "Done" << std::endl;

    return getData();
}

void WaveFunctionCollapseStrategy::generateWallsAndNeighbours(void) {
    for(auto y = 0; y < getHeight(); y++) {
        for(auto x = 0; x < getWidth(); x++) {
            auto& tile = tiles[y][x];

            if(x == 0 || x == getWidth() - 1 || y == 0 || y == getHeight() - 1) {
                tile.entropy = 1;
                tile.possibilities = { 1 };
            }
            else {
                tile.entropy = tileSet.rules.size();
                auto kv = std::views::keys(tileSet.rules);
                tile.possibilities = { kv.begin(), kv.end() };
            }

            tile.x = x;
            tile.y = y;

            addTileNeighbours(x, y, &tiles[y][x]);
        }
    }
}

// TODO: Better way of finding paths between rooms
void WaveFunctionCollapseStrategy::generateRoomsAndPaths(void) {
    std::vector<glm::ivec2> roomCenterPoints;

    for(int i = 0; i < roomConfiguration.numRooms; i++) {
        roomCenterPoints.push_back(addRoom());
    }

    std::sort(roomCenterPoints.begin(), roomCenterPoints.end());

    for(int i = 1; i < roomCenterPoints.size(); i++) {
        auto p1 = roomCenterPoints[i-1];
        auto p2 = roomCenterPoints[i];
        auto intersections = grid->getIntersections(roomCenterPoints[i-1], roomCenterPoints[i]);

        for(auto intersection : intersections) {
            tiles[intersection.y][intersection.x].entropy = 1;
            tiles[intersection.y][intersection.x].possibilities = { 0 };
            tilesToCollapse.push(&tiles[intersection.y][intersection.x]);
        }
    }
}

glm::ivec2 WaveFunctionCollapseStrategy::addRoom(void) {
    int roomSizeX = randomRange(roomConfiguration.minRoomSize.x, roomConfiguration.maxRoomSize.x);
    int roomSizeY = randomRange(roomConfiguration.minRoomSize.y, roomConfiguration.maxRoomSize.y);
    int roomX = randomRange(2, getWidth() - roomSizeX - 2);
    int roomY = randomRange(2, getHeight() - roomSizeY - 2);

    for(int x = roomX; x < roomX + roomSizeX; x++) {
        for(int y = roomY; y < roomY + roomSizeY; y++) {
            tiles[y][x].entropy = 1;
            tiles[y][x].possibilities = { 0 };
            tilesToCollapse.push(&tiles[y][x]);
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
        weights.push_back(tileSet.types[possiblity].weight);
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
        connectors.push_back(tileSet.rules[neighbourPossiblity][direction]);
    }

    Direction opposite;
    if(direction == NORTH) opposite = SOUTH;
    if(direction == SOUTH) opposite = NORTH;
    if(direction == EAST) opposite = WEST;
    if(direction == WEST) opposite = EAST;

    std::erase_if(tile->possibilities, [&](const auto& possiblity) {
        bool reduced = !contains(connectors, tileSet.rules[possiblity][opposite]);
        return reduced;
    });

    tile->entropy = tile->possibilities.size();

    return reduced;
}

std::vector<WaveFunctionCollapseStrategy::WFTile*> WaveFunctionCollapseStrategy::getLowestEntropyTiles(void) {
    std::vector<WFTile*> lowestEntropyTiles;
    auto lowestEntropy = tileSet.rules.size();

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

    // std::cout << "Processing " << lowestEntropyTiles.size() << " tiles" << std::endl;

    if(lowestEntropyTiles.empty()) {
        return true;
    }

    auto tileToCollapse = randomChoice(lowestEntropyTiles);
    collapseTile(tileToCollapse);

    tilesToCollapse.push(tileToCollapse);

    while(!tilesToCollapse.empty()) {
        auto tile = tilesToCollapse.top();
        auto tilePossibilities = tile->possibilities;
        tilesToCollapse.pop();

        for(auto& [direction, neighbour] : tile->neighbours) {
            if(neighbour->entropy != 0 && reduceTile(neighbour, tilePossibilities, direction)) {
                tilesToCollapse.push(neighbour);
            }
        }
    }

    return false;
}

void WaveFunctionCollapseStrategy::overrideTiles(void) {
    for(auto y = 0; y < getHeight(); y++) {
        for(auto x = 0; x < getWidth(); x++) {
            overrideTileId(&tiles[y][x]);

            auto tileId = tiles[y][x].possibilities[0];
            setTile(x, y, { tileId, tileId == 1, false }); // TODO: Better way of assessing whether a tile is walkable
        }
    }
}

void WaveFunctionCollapseStrategy::overrideTileId(WFTile* tile) {
    if(tile->x == 0 || tile->y == 0 || tile->x == getWidth() - 1 || tile->y == getHeight() - 1) {
        return;
    }

    if(tile->possibilities.size() > 1) {
        return;
    }

    auto type = tileSet.types[tile->possibilities[0]].type;

    if(tileSet.variants[type].size() < TileSet::Variant::COUNT) {
        return;
    }

    auto n = tileSet.types[tile->neighbours[NORTH]->possibilities[0]].type;
    auto s = tileSet.types[tile->neighbours[SOUTH]->possibilities[0]].type;
    auto e = tileSet.types[tile->neighbours[EAST]->possibilities[0]].type;
    auto w = tileSet.types[tile->neighbours[WEST]->possibilities[0]].type;

    if(n != type && s != type && e != type && w != type) tile->possibilities[0] = tileSet.variants[type][TileSet::CENTER];

    else if(n == type && s == type && e == type && w == type) tile->possibilities[0] = tileSet.variants[type][TileSet::MIDDLE];

    else if(n != type && s == type && e == type && w == type) tile->possibilities[0] = tileSet.variants[type][TileSet::NORTH];
    else if(n == type && s != type && e == type && w == type) tile->possibilities[0] = tileSet.variants[type][TileSet::SOUTH];
    else if(n == type && s == type && e != type && w == type) tile->possibilities[0] = tileSet.variants[type][TileSet::EAST];
    else if(n == type && s == type && e == type && w != type) tile->possibilities[0] = tileSet.variants[type][TileSet::WEST];
    
    else if(n != type && s == type && e != type && w == type) tile->possibilities[0] = tileSet.variants[type][TileSet::NORTH_EAST];
    else if(n != type && s == type && e == type && w != type) tile->possibilities[0] = tileSet.variants[type][TileSet::NORTH_WEST];
    else if(n == type && s != type && e != type && w == type) tile->possibilities[0] = tileSet.variants[type][TileSet::SOUTH_EAST];
    else if(n == type && s != type && e == type && w != type) tile->possibilities[0] = tileSet.variants[type][TileSet::SOUTH_WEST];

    // TODO: Other possible configurations
}