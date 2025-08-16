#pragma once

#include <set>
#include <vector>
#include <deque>
#include <map>
#include <functional>
#include <ranges>

#include "game/entities/entity.h"
#include "core/util/gameassert.h"
#include "core/util/randomutils.h"
#include "core/event/eventpublisher.h"
#include "game/entities/behaviour/behaviourstrategy.h"
#include "game/application/applicationcontext.h"
#include "game/actions/action.h"
#include "game/event/events.h"
#include "game/actions/moveaction.h"
#include "game/actions/attackaction.h"
#include "game/actions/takeitemaction.h"
#include "game/actions/equipgearaction.h"
#include "game/actions/equipweaponaction.h"
#include "game/items/itemcontroller.h"
#include "game/effects/effectcontroller.h"
#include "game/participant/participant.h"
#include "game/engagements/engagementcontroller.h"

class BehaviourStrategy;
class Participant;
class Engagement;
class EngagementController;

class GameController : public EventPublisher<
    MoveActionEventData,
    AttackActionEventData,
    TakeItemActionEventData,
    EquipItemActionEventData,
    EquipWeaponActionEventData
> {
public:
    GameController();

    void initialise(ApplicationContext& context);
    void update(int64_t timeSinceLastFrame, bool& quit);

    Participant* addParticipant(
        bool isPlayer,
        const std::vector<Entity*>& entities,
        std::unique_ptr<BehaviourStrategy> behaviourStrategy = nullptr,
        bool isReady = true);
    Participant* addParticipant(
        int id,
        bool isPlayer,
        const std::vector<Entity*>& entities, 
        std::unique_ptr<BehaviourStrategy> behaviourStrategy = nullptr,
        bool isReady = true
    );
    void addEntityToParticipant(int participantId, Entity* entity);
    Participant* getParticipant(int id);
    std::vector<Participant*> getParticipants(void);
    bool hasParticipant(int id);

    void reset(void);

    // TODO: Move these and protected functions to an ActionsController class
    bool executeActionImmediately(std::unique_ptr<Action> action);
    bool queueAction(std::unique_ptr<Action> action);

    void setOnAllParticipantsSetFunction(std::function<void()> onAllParticipantsSet);

    void allParticipantsSet(void);

    EngagementController* getEngagementController(void);

protected:
    ApplicationContext* context;
    std::unique_ptr<EngagementController> engagementController;

    bool initialised;

    std::map<int, std::unique_ptr<Participant>> participants;
    std::function<void()> onAllParticipantsSet;

    void executeActions(uint32_t engagementId);
    void executeEntityActions(Engagement* engagement, Entity* entity);
    void endCurrentParticipantTurn(uint32_t engagementId);
    void nextParticipantTurn(uint32_t engagementId);

    virtual bool canProgressToNextTurn(Engagement* engagement) = 0;
    virtual void onParticipantTurnEnd(Engagement* engagement) = 0;
    virtual void additionalUpdate(int64_t timeSinceLastFrame, bool& quit) = 0;

private:
    void publishAction(Action* action);
};