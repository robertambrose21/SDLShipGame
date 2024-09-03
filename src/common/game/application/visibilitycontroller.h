#pragma once

#include <map>

#include "core/grid/grid.h"
#include "game/event/events.h"
#include "applicationcontext.h"

class VisiblityController : 
    public EventPublisher<TilesRevealedEventData>, 
    public EventSubscriber<EntitySetPositionEventData>
{
public:
    typedef struct _revealedTile {
        int x, y;
        bool isVisible;
        Grid::Tile tile;

        bool operator<(const _revealedTile& other) const {
            return x < other.x || (x == other.x && y < other.y);
        }
    } RevealedTile;

    VisiblityController();

    void initialise(ApplicationContext& context);

    void revealTiles(int participantId, const std::vector<glm::ivec2>& tiles);
    void onPublish(const Event<EntitySetPositionEventData>& event);

    const std::map<int, std::set<RevealedTile>>& getRevealedTiles(void) const;
    const std::set<RevealedTile>& getRevealedTiles(int participantId);

private:
    ApplicationContext* context;
    bool initialised;

    std::map<int, std::set<RevealedTile>> revealedTiles;
};
