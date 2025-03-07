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

class TurnController : 
    public EventPublisher<
        TurnEventData, 
        MoveActionEventData,
        AttackActionEventData,
        TakeItemActionEventData,
        EngagementEventData,
        EquipItemActionEventData,
        EquipWeaponActionEventData
    >
{
public:
    TurnController();

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

    // void queueEngagement(int turnNumber, const Engagement& engagement);

    // void engage(int participantIdA, int participantIdB);
    // void disengage(int participantIdA, int participantIdB);

    void reset(void);
    
    // void passParticipant(int id);
    // void setCurrentParticipant(int id);
    // int getCurrentParticipantId(void) const;

    bool executeActionImmediately(std::unique_ptr<Action> action);
    bool queueAction(std::unique_ptr<Action> action);
    // void executeActions(int participantId);

    void addOnNextTurnFunction(std::function<void(int, int)> onNextTurnFunc);
    void setOnAllParticipantsSetFunction(std::function<void()> onAllParticipantsSet);

    void allParticipantsSet(void);

    EngagementController* getEngagementController(void);

    // int getTurnNumber(void) const;
    // void setTurnNumber(int turnNumber);

protected:
    ApplicationContext* context;
    std::unique_ptr<EngagementController> engagementController;

    bool initialised;

    // int turnNumber;
    // int currentParticipantId;

    std::map<int, std::unique_ptr<Participant>> participants;
    std::vector<std::function<void(int, int)>> onNextTurnWorkers;
    std::function<void()> onAllParticipantsSet;

    // void endCurrentParticipantTurn(void);
    // void nextParticipantTurn(void);
    // void executeEntityActions(Entity* entity);
    // void incrementTurn(void);

    void executeActions(Engagement* engagement);
    void executeEntityActions(Engagement* engagement, Entity* entity);
    void endCurrentParticipantTurn(Engagement* engagement);
    void nextParticipantTurn(Engagement* engagement);

    void publishAction(Action& action);

    // void processEngagements();

    // virtual bool canProgressToNextTurn(int participantId) = 0;
    // virtual void onParticipantTurnEnd(int participantId) = 0;
    virtual bool canProgressToNextTurn(Engagement* engagement) = 0;
    virtual void onParticipantTurnEnd(Engagement* engagement) = 0;
    virtual void additionalUpdate(int64_t timeSinceLastFrame, bool& quit) = 0;
};