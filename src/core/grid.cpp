#include "grid.h"

Grid::Grid(int width, int height, std::vector<std::vector<Tile>> data) :
    width(width),
    height(height),
    data(data)
{
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

void Grid::setTile(const int& x, const int& y, Tile tile) {
    data[y][x] = tile;
}

std::vector<std::vector<Tile>> Grid::getData(void) const {
    return data;
}

Tile Grid::getTileAt(const int& x, const int& y) const {
    return data[y][x];
}

std::deque<glm::ivec2> Grid::findPath(const glm::ivec2& source, const glm::ivec2& destination) {
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
        auto currentNode = getLowestFScoreNode(open, fScore);

        if(currentNode == destination) {
            return buildPath(cameFrom, currentNode);
        }

        open.erase(currentNode);
        for(auto neighbour : getNeighbours(currentNode)) {
            auto tentativeGScore = gScore[currentNode] + getDistanceWeight(currentNode, neighbour);

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
    std::set<glm::ivec2> open,
    std::map<glm::ivec2, int> fScore
) const {
    glm::ivec2 lowestScoreNode;
    auto lowestScore = INT_MAX;

    for(auto node : open) {
        if(fScore.contains(node) && fScore[node] < lowestScore) {
            lowestScore = fScore[node];
            lowestScoreNode = node;
        }
    }

    return lowestScoreNode;
}

std::set<glm::ivec2> Grid::getNeighbours(const glm::ivec2& node) {
    std::set<glm::ivec2> neighbours;

    for(int x = -1; x < 2; x++) {
        for(int y = -1; y < 2; y++) {
            auto neighbour = node + glm::ivec2(x, y);

            if(!(x == 0 && y == 0) && isNodeInBounds(neighbour) && getTileAt(node.x, node.y).isWalkable) {
                neighbours.insert(neighbour);
            }
        }
    }

    return neighbours;
}

bool Grid::isNodeInBounds(const glm::ivec2& node) const {
    return node.x >= 0 && node.x < width && node.y >= 0 && node.y < height;
}

std::deque<glm::ivec2> Grid::buildPath(
    std::map<glm::ivec2, glm::ivec2> cameFrom,
    glm::ivec2 currentNode
) {
    std::deque<glm::ivec2> path(1, currentNode);

    while(cameFrom.contains(currentNode)) {
        currentNode = cameFrom[currentNode];
        path.push_front(currentNode);
    }

    return path;
}
