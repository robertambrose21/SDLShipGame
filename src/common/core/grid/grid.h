#pragma once

#include "../glmimport.h"
#include <vector>
#include <iostream>
#include <map>
#include <set>  
#include <deque>
#include <limits.h>
#include <algorithm>

#include "core/util/gameassert.h"
#include "core/util/timing.h"

typedef struct _tile {
    int id;
    bool isWalkable = true;
    int turnsFrozenFor = 0;
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
    bool isTileInBounds(int x, int y);

    // Line intersection
    float pointOnLineSide(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& point);
    bool hasPointsOnDifferentSides(const glm::vec2& p1, const glm::vec2& p2, const std::vector<glm::vec2>& corners);
    bool hasTileIntersection(const glm::vec2& p1, const glm::vec2& p2, int x, int y);

public:
    Grid(int width, int height, const std::vector<std::vector<Tile>>& data = { });

    void nextTurn(void);

    int getWidth(void) const;
    int getHeight(void) const;

    void setTile(int x, int y, const Tile& tile);
    void setTileWalkable(int x, int y, bool isWalkable);
    void setTileFrozenFor(int x, int y, int turnsFrozenFor);

    // Expects points to be in tile space.
    // e.g. the line (0, 0) -> (1, 1) in a 3x3 grid will check a 4x4 square of tiles like so:
    // x x o
    // x x o
    // o o o
    // Where x are the tiles checked and o are unchecked tiles
    bool hasIntersection(const glm::vec2& p1, const glm::vec2& p2);

    const std::vector<std::vector<Tile>>& getData(void) const;
    // TODO: Throw exception if x/y are out of bounds
    const Tile& getTileAt(int x, int y) const;
    std::vector<glm::ivec2> getTilesInCircle(int x, int y, float radius);
    std::vector<glm::ivec2> getTilesInSquare(int x, int y, int w, int h);

    std::deque<glm::ivec2> findPath(const glm::ivec2& source, const glm::ivec2& destination);
};
