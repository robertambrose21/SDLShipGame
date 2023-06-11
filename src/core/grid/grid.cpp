#include "grid.h"

Grid::Grid(int width, int height, const std::vector<std::vector<Tile>>& data) :
    width(width),
    height(height),
    data(data)
{
    game_assert(width > 0);
    game_assert(height > 0);

    if(data.empty()) {
        this->data.resize(height, std::vector<Tile>(width));
    }
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
}

void Grid::setTileWalkable(int x, int y, bool isWalkable) {
    game_assert(x < getWidth() && y < getHeight());
    data[y][x].isWalkable = isWalkable;
}

const std::vector<std::vector<Tile>>& Grid::getData(void) const {
    return data;
}

const Tile& Grid::getTileAt(int x, int y) const {
    game_assert(x < getWidth() && y < getHeight());
    return data[y][x];
}

std::vector<glm::ivec2> Grid::getTilesInCircle(int x, int y, float radius) {
    game_assert(x < getWidth() && y < getHeight());

    int squareHalfSize = std::floor(radius);
    int upperX = std::max(x + squareHalfSize + 1, x);
    int lowerX = std::min(x - squareHalfSize, x);
    int upperY = std::max(y + squareHalfSize + 1, y);
    int lowerY = std::min(y - squareHalfSize, y);

    // Store as float so we can get more accurate distance measurements
    std::vector<glm::vec2> square;

    for(int i = lowerX; i < upperX; i++) {
        for(int j = lowerY; j < upperY; j++) {
            square.push_back(glm::vec2(i, j));
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
            return buildPath(cameFrom, currentNode);
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
    return getTileAt(node.x, node.y).isWalkable;
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
