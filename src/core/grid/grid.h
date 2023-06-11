#pragma once

#include "../glmimport.h"
#include <vector>
#include <iostream>
#include <map>
#include <set>  
#include <deque>
#include <limits.h>

#include "core/util/gameassert.h"

typedef struct _tile {
    int id;
    bool isWalkable;
} Tile;

class Grid {
private:
    int width;
    int height;

    std::vector<std::vector<Tile>> data;

    // Path finding
    int getManhattanDistance(const glm::ivec2& source, const glm::ivec2& destination) const;
    int getDistanceWeight(const glm::ivec2& currentNode, const glm::ivec2& neighbour);
    glm::ivec2 getLowestFScoreNode(
        const std::set<glm::ivec2>& open,
        const std::map<glm::ivec2, int>& fScore
    ) const;
    std::set<glm::ivec2> getNeighbours(const glm::ivec2& node);
    bool isNodeInBounds(const glm::ivec2& node) const;
    bool isNodeWalkable(const glm::ivec2& node) const;
    std::string nodeToString(const glm::ivec2& node) const;
    std::deque<glm::ivec2> buildPath(
        const std::map<glm::ivec2, glm::ivec2>& cameFrom,
        glm::ivec2 currentNode
    );

    bool isTileInRange(int x, int y, const glm::vec2& position, float distance);

public:
    Grid(int width, int height, const std::vector<std::vector<Tile>>& data = { });

    int getWidth(void) const;
    int getHeight(void) const;

    void setTile(int x, int y, const Tile& tile);
    void setTileWalkable(int x, int y, bool isWalkable);

    const std::vector<std::vector<Tile>>& getData(void) const;
    // TODO: Throw exception if x/y are out of bounds
    const Tile& getTileAt(int x, int y) const;
    std::vector<glm::ivec2> getTilesInCircle(int x, int y, float radius);

    std::deque<glm::ivec2> findPath(const glm::ivec2& source, const glm::ivec2& destination);
};
