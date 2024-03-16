#pragma once

#include <set>
#include <vector>
#include <deque>
#include <map>
#include <functional>
#include <ranges>

#include "game/entities/entity.h"
#include "core/util/gameassert.h"
#include "core/util/randomrolls.h"
#include "core/event/eventpublisher.h"
#include "game/entities/behaviour/behaviourstrategy.h"
#include "game/application/applicationcontext.h"
#include "game/actions/action.h"
#include "game/event/events.h"
#include "game/actions/moveaction.h"
#include "game/actions/attackaction.h"
#include "game/actions/takeitemaction.h"
#include "game/actions/equipitemaction.h"
#include "game/actions/equipweaponaction.h"
#include "game/items/itemcontroller.h"
#include "game/effects/effectcontroller.h"

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
    // TODO: This should probably be moved to a separate class soon
    // TODO: Consider making entities a map rather than a set
    typedef struct _participant {
        int id;
        bool isReady;
        bool isPlayer;
        std::vector<Entity*> entities;
        std::vector<Item*> items;
        bool passNextTurn;
        std::unique_ptr<BehaviourStrategy> behaviourStrategy;
        std::set<int> engagements;
    } Participant;

protected:
    ApplicationContext* context;
    bool initialised;

    int turnNumber;
    int currentParticipantId;

    std::map<int, std::unique_ptr<Participant>> participants;
    std::vector<std::function<void(int, int)>> onNextTurnWorkers;
    std::function<void()> onAllParticipantsSet;

    void endCurrentParticipantTurn(void);
    void nextParticipantTurn(void);
    void executeEntityActions(Entity* entity);
    void incrementEntitiesTurn(std::vector<Entity*> entities);
    void incrementTurn(void);
    void publishAction(Action& action);

    virtual bool canProgressToNextTurn(int participantId) = 0;
    virtual void additionalUpdate(int64_t timeSinceLastFrame, bool& quit) = 0;

public:
    TurnController();

    void initialise(ApplicationContext& context);
    void update(int64_t timeSinceLastFrame, bool& quit);

    Participant* addParticipant(
        bool isPlayer,
        const std::vector<Entity*>& entities, 
        std::unique_ptr<BehaviourStrategy> behaviourStrategy = nullptr,
        bool isReady = true
    );
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

    void engage(int participantIdA, int participantIdB);
    void disengage(int participantIdA, int participantIdB);

    void reset(void);
    
    void passParticipant(int id);
    void setCurrentParticipant(int id);
    int getCurrentParticipant(void) const;

    bool queueAction(std::unique_ptr<Action> action);
    void executeActions(int participantId);

    void addOnNextTurnFunction(std::function<void(int, int)> onNextTurnFunc);
    void setOnAllParticipantsSetFunction(std::function<void()> onAllParticipantsSet);

    void allParticipantsSet(void);

    int getTurnNumber(void) const;
};