#include "grid.h"

Grid::Grid(int width, int height, const std::vector<std::vector<Tile>>& data) :
    width(width),
    height(height),
    data(data),
    isDirty(true)
{
    game_assert(width > 0);
    game_assert(height > 0);

    if(data.empty()) {
        this->data.resize(height, std::vector<Tile>(width));
    }
}

void Grid::nextTurn(void) {
    for(auto y = 0; y < getHeight(); y++) {
        for(auto x = 0; x < getWidth(); x++) {
            data[y][x].isFrozen = false;
        }
    }

    setDirty(true);
}

int Grid::getWidth(void) const {
    return width;
}

int Grid::getHeight(void) const {
    return height;
}

void Grid::setTile(int x, int y, const Tile& tile) {
    game_assert(x < getWidth() && y < getHeight());
    data[y][x] = tile;
    setDirty(true);

    publish<TileEventData>({ x, y, tile.id, tile.isWalkable, tile.isFrozen });
}

void Grid::setTileWalkable(int x, int y, bool isWalkable) {
    game_assert(x < getWidth() && y < getHeight());
    data[y][x].isWalkable = isWalkable;
    setDirty(true);
}

void Grid::setTileFrozen(int x, int y, bool isFrozen) {
    game_assert(x < getWidth() && y < getHeight());
    data[y][x].isFrozen = isFrozen;
    setDirty(true);
}

bool Grid::getIsDirty(void) const {
    return isDirty;
}

void Grid::setDirty(bool isDirty) {
    this->isDirty = isDirty;
    publish<GridDirtyEventData>({ isDirty });
}

void Grid::setData(const std::vector<std::vector<Tile>>& data) {
    this->data = data;
    setDirty(true);
}

const std::vector<std::vector<Grid::Tile>>& Grid::getData(void) const {
    return data;
}

std::vector<Grid::Tile> Grid::getWalkableTiles(void) {
    if(isDirty) {
        walkableTiles.clear();

        for(auto y = 0; y < getHeight(); y++) {
            for(auto x = 0; x < getWidth(); x++) {
                if(data[y][x].isWalkable) {
                    walkableTiles.push_back(data[y][x]);
                }
            }
        }

        setDirty(false);
    }

    return walkableTiles;
}

const Grid::Tile& Grid::getTileAt(int x, int y) const {
    game_assert(x >= 0 && y >= 0);
    game_assert(x < getWidth() && y < getHeight());
    return data[y][x];
}

std::vector<glm::ivec2> Grid::getTilesInCircle(int x, int y, float radius) {
    if(!isTileInBounds(x, y)) {
        return std::vector<glm::ivec2>();
    }

    int squareHalfSize = std::floor(radius);
    int upperX = std::max(x + squareHalfSize + 1, x);
    int lowerX = std::min(x - squareHalfSize, x);
    int upperY = std::max(y + squareHalfSize + 1, y);
    int lowerY = std::min(y - squareHalfSize, y);

    // Store as float so we can get more accurate distance measurements
    std::vector<glm::vec2> square;

    for(int i = lowerX; i < upperX; i++) {
        for(int j = lowerY; j < upperY; j++) {
            if(isTileInBounds(i, j)) {
                square.push_back(glm::vec2(i, j));
            }
        }
    }

    std::vector<glm::ivec2> tilePositions;
    
    for(auto const& position : square) {
        if(isTileInRange(x, y, position, radius)) {
            tilePositions.push_back(position);
        }
    }

    return tilePositions;
}

std::vector<glm::ivec2> Grid::getTilesInSquare(int x, int y, int w, int h) {
    game_assert(x < getWidth() && y < getHeight());

    std::vector<glm::ivec2> tiles;

    for(int i = x; i <= x + w; i++) {
        for(int j = y; j <= y + h; j++) {
            tiles.push_back(glm::vec2(i, j));
        }
    }

    return tiles;
}

bool Grid::isTileInRange(int x, int y, const glm::vec2& position, float distance) {
    game_assert(x < getWidth() && y < getHeight());

    glm::vec2 corners[] = {
        glm::vec2(x - .5f, y - .5f),
        glm::vec2(x - .5f, y + .5f),
        glm::vec2(x + .5f, y - .5f),
        glm::vec2(x + .5f, y + .5f)
    };

    for(auto const& corner : corners) {
        if(glm::distance(corner, position) <= distance) {
            return true;
        }
    }

    return false;
}

bool Grid::isTileInBounds(int x, int y) {
    return x >= 0 && y >= 0 && x < getWidth() && y < getHeight();
}

bool Grid::hasIntersection(const glm::vec2& p1, const glm::vec2& p2) {
    // Offset so we get the center of the tile
    auto op1 = p1 + glm::vec2(.5f, .5f);
    auto op2 = p2 + glm::vec2(.5f, .5f);

    int xMin = std::max(0, (int) std::floor(std::min(op1.x, op2.x)));
    int xMax = std::min(getWidth(), (int) std::ceil(std::max(op1.x, op2.x)));
    int yMin = std::max(0, (int) std::floor(std::min(op1.y, op2.y)));
    int yMax = std::min(getHeight(), (int) std::ceil(std::max(op1.y, op2.y)));

    for(int x = xMin; x < xMax; x++) {
        for(int y = yMin; y < yMax; y++) {
            if(!data[y][x].isWalkable && hasTileIntersection(op1, op2, x, y)) {
                return true;
            }
        }
    }

    return false;
}

bool Grid::hasTileIntersection(const glm::vec2& p1, const glm::vec2& p2, int x, int y) {
    std::vector<glm::vec2> corners = {
        glm::vec2(x, y),
        glm::vec2(x, y + 1),
        glm::vec2(x + 1, y),
        glm::vec2(x + 1, y + 1)
    };

    if(!hasPointsOnDifferentSides(p1, p2, corners)) {
        return false;
    }
    else if(p1.x > corners[3].x && p2.x > corners[3].x) {
        return false;
    }
    else if(p1.x < corners[0].x && p2.x < corners[0].x) {
        return false;
    }
    else if(p1.y > corners[3].y && p2.y > corners[3].y) {
        return false;
    }
    else if(p1.y < corners[0].y && p2.y < corners[0].y) {
        return false;
    }
    
    return true;
}

bool Grid::hasPointsOnDifferentSides(const glm::vec2& p1, const glm::vec2& p2, const std::vector<glm::vec2>& corners) {
    game_assert(corners.size() == 4);

    float p = pointOnLineSide(p1, p2, corners[0]);

    for(int i = 1; i < 4; i++) {
        p *= pointOnLineSide(p1, p2, corners[i]);
        if(p < 0) {
            return true;
        }
    }

    return false;
}

float Grid::pointOnLineSide(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& point) {
    return ((p2.y - p1.y) * point.x) + ((p1.x - p2.x) * point.y) + (p2.x * p1.y - p1.x * p2.y); 
}

std::deque<glm::ivec2> Grid::findPath(const glm::ivec2& source, const glm::ivec2& destination) {
    if(!isNodeWalkable(source) || !isNodeWalkable(destination)) {
        return std::deque<glm::ivec2>();
    }

    if(!isNodeInBounds(source) || !isNodeInBounds(destination)) {
        std::cout << "Path exceeded grid boundaries, no path found. source: (" 
            << source.x
            << ", "
            << source.y
            << "), destination: ("
            << destination.x
            << ", "
            << destination.y
            << ")"
            << std::endl;
        return std::deque<glm::ivec2>();
    }

    auto startTime = getCurrentTimeInMilliseconds();

    auto cameFrom = std::map<glm::ivec2, glm::ivec2>();
    auto open = std::set<glm::ivec2>();
    open.insert(source);

    auto gScore = std::map<glm::ivec2, int>();
    gScore[source] = 0;

    auto fScore = std::map<glm::ivec2, int>();
    fScore[source] = getManhattanDistance(source, destination);

    while(!open.empty()) {
        auto const& currentNode = getLowestFScoreNode(open, fScore);

        if(currentNode == destination) {
            auto path = buildPath(cameFrom, currentNode);
            auto timeTaken = getCurrentTimeInMilliseconds() - startTime;

            // std::cout << "Path time taken: " << timeTaken << std::endl;

            return path;
        }

        open.erase(currentNode);
        for(auto const& neighbour : getNeighbours(currentNode)) {
            auto const& tentativeGScore = gScore[currentNode] + getDistanceWeight(currentNode, neighbour);

            if(!gScore.contains(neighbour) || tentativeGScore < gScore[neighbour]) {
                cameFrom[neighbour] = currentNode;
                gScore[neighbour] = tentativeGScore;
                fScore[neighbour] = gScore[neighbour] + getManhattanDistance(neighbour, destination);

                if(!open.contains(neighbour)) {
                    open.insert(neighbour);
                }
            }
        }
    }

    std::cout << "No path found between: "
        << source.x
        << ", "
        << source.y
        << ") and ("
        << destination.x
        << ", "
        << destination.y
        << ")"
        << std::endl;
    return std::deque<glm::ivec2>();
}

int Grid::getManhattanDistance(const glm::ivec2& source, const glm::ivec2& destination) const {
    return std::abs(source.x - destination.x) + std::abs(source.y - destination.y);
}

int Grid::getDistanceWeight(const glm::ivec2& currentNode, const glm::ivec2& neighbour) {
    return currentNode.x != neighbour.x && currentNode.y != neighbour.y ? 14 : 10;
}

glm::ivec2 Grid::getLowestFScoreNode(
    const std::set<glm::ivec2>& open,
    const std::map<glm::ivec2, int>& fScore
) const {
    glm::ivec2 lowestScoreNode;
    auto lowestScore = INT_MAX;

    for(auto const& node : open) {
        auto const& potentialNode = fScore.find(node);

        if(potentialNode != fScore.end() && potentialNode->second < lowestScore) {
            lowestScore = potentialNode->second;
            lowestScoreNode = node;
        }
    }

    return lowestScoreNode;
}

std::set<glm::ivec2> Grid::getNeighbours(const glm::ivec2& node) {
    std::set<glm::ivec2> neighbours;

    for(int x = -1; x < 2; x++) {
        for(int y = -1; y < 2; y++) {
            auto const& neighbour = node + glm::ivec2(x, y);

            if(!(x == 0 && y == 0) && isNodeInBounds(neighbour) && isNodeWalkable(node)) {
                neighbours.insert(neighbour);
            }
        }
    }

    return neighbours;
}

bool Grid::isNodeInBounds(const glm::ivec2& node) const {
    return node.x >= 0 && node.x < width && node.y >= 0 && node.y < height;
}

bool Grid::isNodeWalkable(const glm::ivec2& node) const {
    auto tile = getTileAt(node.x, node.y);

    return tile.isWalkable && !tile.isFrozen;
}

std::deque<glm::ivec2> Grid::buildPath(
    const std::map<glm::ivec2, glm::ivec2>& cameFrom,
    glm::ivec2 currentNode
) {
    std::deque<glm::ivec2> path(1, currentNode);

    while(cameFrom.contains(currentNode)) {
        currentNode = cameFrom.at(currentNode);
        path.push_front(currentNode);
    }

    return path;
}
