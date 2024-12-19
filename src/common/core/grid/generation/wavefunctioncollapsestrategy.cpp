#include "wavefunctioncollapsestrategy.h"

WaveFunctionCollapseStrategy::WaveFunctionCollapseStrategy(
    Grid* grid,
    const TileSet& tileSet,
    const RoomConfiguration& roomConfiguration
) :
    GenerationStrategy(grid->getWidth(), grid->getHeight(), roomConfiguration),
    grid(grid),
    isCollapsed(false),
    tileSet(tileSet)
{ }

std::vector<std::vector<Grid::Tile>> WaveFunctionCollapseStrategy::generate(void) {
    std::cout << "Generating map... " << std::endl;

    int chunkSize = 8;
    int numRetries = 100;
    auto chunks = generateChunks(chunkSize);
    bool isDone = false;

    while(!isDone) {
        tiles.resize(grid->getWidth(), std::vector<WFTile>(grid->getHeight()));
        tilesToCollapse = {};

        generateWallsAndNeighbours();
        generateRoomsAndPaths();

        int numChunks = 0;
        bool failed = false;
    
        while(!failed && numChunks < chunks.size()) {
            auto chunk = chunks[numChunks];
            failed = subgenerate(chunkSize, numRetries, chunk, numChunks);

            numChunks++;
        }

        if(failed) {
            std::cout << "Failed" << std::endl;
        }
        else {
            isDone = true;
        }
    }

    overrideTiles();

    std::cout << "Done" << std::endl;

    return getData();
}

bool WaveFunctionCollapseStrategy::subgenerate(
    int chunkSize, 
    int numRetries, 
    WaveFunctionCollapseStrategy::WFChunk& chunk, 
    int numChunks
) {
    bool failed = false;
    int retries = 0;
    int tilesToGenerate = ((chunk.xMax - chunk.xMin) + 1) * ((chunk.yMax - chunk.yMin) + 1);
    int initialCollapse = 0;
    tilesCollapsed = 0;

    for(auto y = chunk.yMin; y <= chunk.yMax; y++) {
        for(auto x = chunk.xMin; x <= chunk.xMax; x++) {
            auto& tile = tiles[y][x];

            if(x == chunk.xMin || x == chunk.xMax || y == chunk.yMin || y == chunk.yMax) {
                tile.entropy = 1; // Reopen borders
            }
            else if(tile.seeded) {
                tile.entropy = 1;
            } 
            else {
                tile.entropy = tileSet.getRules().size();
                auto kv = std::views::keys(tileSet.getRules());
                tile.possibilities = { kv.begin(), kv.end() };
            }
        }
    }

    while (tilesCollapsed < tilesToGenerate && retries < numRetries) {
        tilesSnapshot = tiles;
        bool isCollapsed = false;
        tilesCollapsed = initialCollapse;
        tilesToCollapse = {};

        while (!isCollapsed) {
            isCollapsed = collapse(chunk);
        }

        retries++;

        if (tilesCollapsed < tilesToGenerate) {
            tiles = tilesSnapshot;
            failed = true;
        } else {
            failed = false;
        }
    }

    std::cout
        << "Chunk ["
        << numChunks
        << "] done: "
        << tilesCollapsed
        << "/"
        << tilesToGenerate
        << " retries "
        << retries
        << std::endl;

    return failed;
}

std::vector<WaveFunctionCollapseStrategy::WFChunk> WaveFunctionCollapseStrategy::generateChunks(int chunkSize) {
    std::vector<WFChunk> chunks;

    int chunksX = getWidth() / chunkSize;
    int chunksY = getHeight() / chunkSize;

    int lastChunkX = getWidth() % chunkSize;
    int lastChunkY = getHeight() % chunkSize;

    if(lastChunkX > 0) chunksX++;
    if(lastChunkY > 0) chunksY++;

    for(int i = 0; i < chunksX * 2; i++) {

        int xMin = i * (chunkSize / 2);
        int xMax = std::min(xMin + (chunkSize - 1), getWidth() - 1);

        for(int j = 0; j < chunksY * 2; j++) {
            int yMin = j * (chunkSize / 2);
            int yMax = std::min(yMin + (chunkSize - 1), getHeight() - 1);

            chunks.push_back({ xMin, xMax, yMin, yMax });
        }
    }

    return chunks;
}

void WaveFunctionCollapseStrategy::generateWallsAndNeighbours(void) {
    for(auto y = 0; y < getHeight(); y++) {
        for(auto x = 0; x < getWidth(); x++) {
            auto& tile = tiles[y][x];

            if(x == 0 && y == 0) tile.possibilities = { 26 };
            else if(x == getWidth() - 1 && y == 0) tile.possibilities = { 27 };
            else if(x == 0 && y == getHeight() - 1) tile.possibilities = { 24 };
            else if(x == getWidth() - 1 && y == getHeight() - 1) tile.possibilities = { 25 };
            else if(x == 0) tile.possibilities = { 18 };
            else if(x == getWidth() - 1) tile.possibilities = { 19 };
            else if(y == 0) tile.possibilities = { 17 };
            else if(y == getHeight() - 1) tile.possibilities = { 16 };
            else tile.possibilities = { 1 };

            tile.entropy = 0;

            tile.x = x;
            tile.y = y;

            addTileNeighbours(x, y, &tiles[y][x]);
        }
    }
}

// TODO: Better way of finding paths between rooms
void WaveFunctionCollapseStrategy::generateRoomsAndPaths(void) {
    std::vector<glm::ivec2> roomCenterPoints;

    for(int i = 0; i < getRoomConfiguration().numRooms; i++) {
        auto room = generateRoom(getRooms());
        addRoom(room);

        roomCenterPoints.push_back(
            glm::ivec2(
                randomRange(room.min.x, room.max.x),
                randomRange(room.min.y, room.max.y)
            )
        );
    }

    std::sort(roomCenterPoints.begin(), roomCenterPoints.end());

    for(int i = 1; i < roomCenterPoints.size(); i++) {
        auto p1 = roomCenterPoints[i-1];
        auto p2 = roomCenterPoints[i];
        auto intersections = grid->getIntersections(roomCenterPoints[i-1], roomCenterPoints[i]);

        for(auto intersection : intersections) {
            tiles[intersection.y][intersection.x].entropy = 0;
            tiles[intersection.y][intersection.x].possibilities = { 1 };
            tiles[intersection.y][intersection.x].seeded = true;
        }
    }
}

WaveFunctionCollapseStrategy::Room WaveFunctionCollapseStrategy::generateRoom(const std::vector<Room>& existingRooms) {
    Room room;
    bool isValid = false;

    while(!isValid) {
        room = createRandomRoom();

        if(!hasCollision(room, existingRooms) && isSparse(room, existingRooms)) {
            isValid = true;
        }
    }

    for(int x = room.min.x; x <= room.max.x; x++) {
        for(int y = room.min.y; y <= room.max.y; y++) {
            auto& tile = tiles[y][x];

            tile.possibilities = { 1 };
            tile.entropy = 0;
            tile.seeded = true;
        }
    }

    return room;
}

WaveFunctionCollapseStrategy::Room WaveFunctionCollapseStrategy::createRandomRoom(void) {
    int roomSizeX = randomRange(getRoomConfiguration().minRoomSize.x, getRoomConfiguration().maxRoomSize.x);
    int roomSizeY = randomRange(getRoomConfiguration().minRoomSize.y, getRoomConfiguration().maxRoomSize.y);

    // TODO: Hardcoded 4 is a bit weird
    int roomX = randomRange(1, getWidth() - roomSizeX - 1);
    int roomY = randomRange(1, getHeight() - roomSizeY - 1);

    return { glm::ivec2(roomX, roomY), glm::ivec2(roomX + roomSizeX, roomY + roomSizeY) };
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
        weights.push_back(tileSet.getTile(possiblity).weight);
    }

    tilesCollapsed++;
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
        connectors.push_back(tileSet.getRule(neighbourPossiblity, direction));
    }

    Direction opposite;
    if(direction == NORTH) opposite = SOUTH;
    if(direction == SOUTH) opposite = NORTH;
    if(direction == EAST) opposite = WEST;
    if(direction == WEST) opposite = EAST;

    std::erase_if(tile->possibilities, [&](const auto& possiblity) {
        bool reduced = !contains(connectors, tileSet.getRule(possiblity, opposite));
        return reduced;
    });

    tile->entropy = tile->possibilities.size();

    return reduced;
}

std::vector<WaveFunctionCollapseStrategy::WFTile*> WaveFunctionCollapseStrategy::getLowestEntropyTiles(const WFChunk& chunk) {
    std::vector<WFTile*> lowestEntropyTiles;
    auto lowestEntropy = tileSet.getRules().size();

    for(auto y = chunk.yMin; y <= chunk.yMax; y++) {
        for(auto x = chunk.xMin; x <= chunk.xMax; x++) {
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

bool WaveFunctionCollapseStrategy::collapse(const WFChunk& chunk) {
    auto lowestEntropyTiles = getLowestEntropyTiles(chunk);

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

            if(tiles[y][x].possibilities.size() > 1) {
                setTile(x, y, { 1, true, false });
                continue;
            }

            setTile(x, y, { tileId, tileSet.isWalkable(tileId), false });
        }
    }
}

void WaveFunctionCollapseStrategy::overrideTileId(WFTile* tile) {
    if(tile->possibilities.size() > 1) {
        return;
    }

    auto type = tileSet.getTile(tile->possibilities[0]).type;

    if(tileSet.getNumVariantsForType(type) < TileSet::Variant::COUNT) {
        return;
    }

    if(overrideCornerTileId(tile, type) || overrideEdgeTileId(tile, type)) {
        return;
    }

    auto n = tileSet.getTile(tile->neighbours[NORTH]->possibilities[0]).type;
    auto s = tileSet.getTile(tile->neighbours[SOUTH]->possibilities[0]).type;
    auto e = tileSet.getTile(tile->neighbours[EAST]->possibilities[0]).type;
    auto w = tileSet.getTile(tile->neighbours[WEST]->possibilities[0]).type;
    auto ne = tileSet.getTile(tile->neighbours[NORTH]->neighbours[EAST]->possibilities[0]).type;
    auto nw = tileSet.getTile(tile->neighbours[NORTH]->neighbours[WEST]->possibilities[0]).type;
    auto se = tileSet.getTile(tile->neighbours[SOUTH]->neighbours[EAST]->possibilities[0]).type;
    auto sw = tileSet.getTile(tile->neighbours[SOUTH]->neighbours[WEST]->possibilities[0]).type;

    if(n != type && s != type && e != type && w != type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::CENTER);

    else if(n != type && s == type && e == type && w == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::NORTH);
    else if(n == type && s != type && e == type && w == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::SOUTH);
    else if(n == type && s == type && e != type && w == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::EAST);
    else if(n == type && s == type && e == type && w != type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::WEST);
    
    else if(n != type && s == type && e != type && w == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::NORTH_EAST);
    else if(n != type && s == type && e == type && w != type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::NORTH_WEST);
    else if(n == type && s != type && e != type && w == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::SOUTH_EAST);
    else if(n == type && s != type && e == type && w != type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::SOUTH_WEST);

    else if(ne != type && n == type && e == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::NORTH_EAST_2);
    else if(nw != type && n == type && w == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::NORTH_WEST_2);
    else if(se != type && s == type && e == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::SOUTH_EAST_2);
    else if(sw != type && s == type && w == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::SOUTH_WEST_2);

    else if(n == type && s == type && e == type && w == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::MIDDLE);

    // TODO: Other possible configurations
}

bool WaveFunctionCollapseStrategy::overrideCornerTileId(WFTile* tile, const std::string& type) {
    if(tile->x == 0 && tile->y == 0) {
        auto se = tileSet.getTile(tile->neighbours[SOUTH]->neighbours[EAST]->possibilities[0]).type;
        
        if(se == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::MIDDLE);
        else tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::SOUTH_EAST_2);

        return true;
    }

    if(tile->x == 0 && tile->y == getHeight() - 1) {
        auto ne = tileSet.getTile(tile->neighbours[NORTH]->neighbours[EAST]->possibilities[0]).type;
        
        if(ne == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::MIDDLE);
        else tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::NORTH_EAST_2);

        return true;
    }

    if(tile->x == getWidth() - 1 && tile->y == 0) {
        auto sw = tileSet.getTile(tile->neighbours[SOUTH]->neighbours[WEST]->possibilities[0]).type;
        
        if(sw == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::MIDDLE);
        else tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::SOUTH_WEST_2);

        return true;
    }

    if(tile->x == getWidth() - 1 && tile->y == getHeight() - 1) {
        auto nw = tileSet.getTile(tile->neighbours[NORTH]->neighbours[WEST]->possibilities[0]).type;
        
        if(nw == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::MIDDLE);
        else tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::NORTH_WEST_2);

        return true;
    }

    return false;
}

bool WaveFunctionCollapseStrategy::overrideEdgeTileId(WFTile* tile, const std::string& type) {
    if(tile->x == 0) {
        auto e = tileSet.getTile(tile->neighbours[EAST]->possibilities[0]).type;

        if(e == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::MIDDLE);
        else tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::EAST);

        return true;
    }

    if(tile->x == getWidth() - 1) {
        auto w = tileSet.getTile(tile->neighbours[WEST]->possibilities[0]).type;

        if(w == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::MIDDLE);
        else tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::WEST);

        return true;
    }

    if(tile->y == 0) {
        auto e = tileSet.getTile(tile->neighbours[SOUTH]->possibilities[0]).type;

        if(e == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::MIDDLE);
        else tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::SOUTH);

        return true;
    }

    if(tile->y == getHeight() - 1) {
        auto w = tileSet.getTile(tile->neighbours[NORTH]->possibilities[0]).type;

        if(w == type) tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::MIDDLE);
        else tile->possibilities[0] = tileSet.getTileVariantIdForType(type, TileSet::NORTH);

        return true;
    }

    return false;
}
