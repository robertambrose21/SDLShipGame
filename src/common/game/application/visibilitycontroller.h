#pragma once

#include <map>
#include <unordered_set>

#include "core/grid/grid.h"
#include "game/event/events.h"
#include "applicationcontext.h"
#include "game/entities/entitypool.h"
#include "core/util/setutils.h"

class VisiblityController : 
    public EventPublisher<TilesRevealedEventData, EntityVisibilityToParticipantData>, 
    public EventSubscriber<EntitySetPositionEventData>
{
private:
    typedef struct _tileWithVisibility {
        int x, y;
        bool isVisible;
        Grid::Tile tile;

        bool operator<(const _tileWithVisibility& other) const {
            return x < other.x || (x == other.x && y < other.y);
        }
    } TileWithVisibility;

public:
    VisiblityController();

    void initialise(ApplicationContext& context);

    void revealTiles(int participantId, const std::vector<glm::ivec2>& tiles);
    void onPublish(const Event<EntitySetPositionEventData>& event);

    bool isVisible(Entity* entity, Entity* target);

    const std::map<int, std::set<TileWithVisibility>>& getTilesWithVisibility(void) const;
    const std::set<TileWithVisibility>& getTilesWithVisibility(int participantId);
    const std::map<int, std::unordered_set<glm::ivec2, glm::ivec2Hash>>& getVisibleTiles(void) const;
    const std::unordered_set<glm::ivec2, glm::ivec2Hash>& getVisibleTiles(int participantId);

private:
    ApplicationContext* context;
    bool initialised;

    std::map<int, std::unordered_set<glm::ivec2, glm::ivec2Hash>> visibleTiles;
    std::map<int, std::set<TileWithVisibility>> revealedTiles;

    void assignVisibility(
        Entity* entity, 
        Entity* other, 
        float distanceBetweenEntities,
        const std::unordered_set<glm::ivec2, glm::ivec2Hash>& visibleTiles
    );
};
