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
    std::cout << "Generating map... " << std::endl;

    auto wfcTiles = tileSet.getTiles();
    auto neighbours = tileSet.getNeighbours();
    auto walkableTiles = tileSet.getWalkableTiles();

    auto seed = randomRange(0, INT_MAX);
    TilingWFC<WFCTileSet::WFCTile> wfc(wfcTiles, neighbours, getHeight(), getWidth(), { false }, seed);

    generateMapEdge(wfc);
    generateRoomsAndPaths(wfc);

    auto success = wfc.run();

    if(!success.has_value()) {
        std::cout << "Failed generating map" << std::endl;
        return getData();
    }

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

    std::cout << "Done, seed: " << seed << std::endl;
    return getData();
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
    std::vector<glm::ivec2> roomCenterPoints;

    for(int i = 0; i < getRoomConfiguration().numRooms; i++) {
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