#pragma once

#include <map>
#include <unordered_set>

#include "core/grid/grid.h"
#include "game/event/events.h"
#include "applicationcontext.h"
#include "game/actors/actorpool.h"
#include "core/util/setutils.h"

class VisiblityController : 
    public EventPublisher<TilesRevealedEventData, ActorVisibilityToParticipantData>, 
    public EventSubscriber<ActorSetPositionEventData>
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
    void onPublish(const Event<ActorSetPositionEventData>& event);

    bool isVisible(Actor* actor, Actor* target);

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
        Actor* actor, 
        Actor* other, 
        float distanceBetweenActors,
        const std::unordered_set<glm::ivec2, glm::ivec2Hash>& visibleTiles
    );
};
