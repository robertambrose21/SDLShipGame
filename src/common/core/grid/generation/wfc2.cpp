#include "wfc2.h"

WFC2::WFC2(
    Grid* grid,
    const RoomConfiguration& roomConfiguration,
    const WFCTileSet& tileSet
) : 
    GenerationStrategy(grid->getWidth(), grid->getHeight(), roomConfiguration),
    tileSet(tileSet),
    grid(grid)
{ }

std::vector<std::vector<Grid::Tile>> WFC2::generate(void) {
    auto startTime = getCurrentTimeInMicroseconds();
    std::cout << std::format("Generating map ({}, {})... ", getWidth(), getHeight()) << std::endl;

    int numAttempts = 10;
    int successfulAttempt = 0;
    int seed = 0;
    auto success = run(numAttempts, successfulAttempt, seed);

    if(!success.has_value()) {
        return getData();
    }

    auto timeTaken = (getCurrentTimeInMicroseconds() - startTime) / 1000.0;
    std::cout << std::format(
        "Map generation done ({}ms, {}/{} attempts) [seed={}]", 
        timeTaken, 
        successfulAttempt,
        numAttempts, 
        seed
    ) << std::endl;

    std::cout << "Setting tiles... ";
    for(int x = 0; x < getWidth(); x++) {
        for(int y = 0; y < getHeight(); y++) {
            auto const& wfcTile = (*success).data[y * getWidth() + x];
            
            setTile(x, y, { 
                (int) wfcTile.id, 
                tileSet.isTileWalkable(wfcTile.id),
                false, 
                wfcTile.orientation 
            });
        }
    }

    std::cout << "Done" << std::endl;

    return getData();
}

std::optional<Array2D<WFCTileSet::WFCTile>> WFC2::run(
    int numAttempts, 
    int& successfulAttempt, 
    int& seed
) {
    auto wfcTiles = tileSet.getTiles();
    auto neighbours = tileSet.getNeighbours();
    auto walkableTiles = tileSet.getWalkableTiles();

    for(int i = 0; i < numAttempts; i++) {
        seed = randomRange(0, INT_MAX);
        auto success = runAttempt(seed);

        if(success.has_value()) {
            successfulAttempt = i + 1;
            return success;
        }

        std::cout 
            << std::format("Failed to generate map with seed {}, retrying ({} of {} attempts)", seed, i, numAttempts)
            << std::endl;
    }

    std::cout << std::format("Failed to generate map after {} attempts", numAttempts) << std::endl;
    return std::nullopt;
}

std::optional<Array2D<WFCTileSet::WFCTile>> WFC2::runAttempt(int seed) {
    auto wfcTiles = tileSet.getTiles();
    auto neighbours = tileSet.getNeighbours();
    auto walkableTiles = tileSet.getWalkableTiles();

    TilingWFC<WFCTileSet::WFCTile> wfc(wfcTiles, neighbours, getHeight(), getWidth(), { false }, seed);

    generateMapEdge(wfc);
    generateRoomsAndPaths(wfc);

    return wfc.run();
}

void WFC2::generateMapEdge(TilingWFC<WFCTileSet::WFCTile>& wfc) {
    for(int y = 0; y < getHeight(); y++) {
        for(int x = 0; x < getWidth(); x++) {
            if(x == 0 || y == 0 || x == getWidth() - 1 || y == getHeight() - 1) {
                wfc.set_tile(tileSet.getEdgeTile(), 0, y, x);
            }
        }
    }
}

void WFC2::generateRoomsAndPaths(TilingWFC<WFCTileSet::WFCTile>& wfc) {
    auto numRooms = getRoomConfiguration().numRooms;
    clearRooms();

    std::vector<glm::ivec2> roomCenterPoints;

    for(int i = 0; i < numRooms; i++) {
        auto room = generateRoom(wfc, getRooms());
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
            wfc.set_tile(tileSet.getRoomTile(), 0, intersection.y, intersection.x);
        }
    }
}

GenerationStrategy::Room WFC2::generateRoom(
    TilingWFC<WFCTileSet::WFCTile>& wfc, 
    const std::vector<Room>& existingRooms
) {
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
            wfc.set_tile(tileSet.getRoomTile(), 0, y, x);
        }
    }

    return room;
}

GenerationStrategy::Room WFC2::createRandomRoom(void) {
    int roomSizeX = randomRange(getRoomConfiguration().minRoomSize.x, getRoomConfiguration().maxRoomSize.x);
    int roomSizeY = randomRange(getRoomConfiguration().minRoomSize.y, getRoomConfiguration().maxRoomSize.y);

    int roomX = randomRange(1, getWidth() - roomSizeX - 1);
    int roomY = randomRange(1, getHeight() - roomSizeY - 1);

    return { glm::ivec2(roomX, roomY), glm::ivec2(roomX + roomSizeX, roomY + roomSizeY) };
}