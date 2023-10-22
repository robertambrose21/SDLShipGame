#pragma once

#include <set>
#include <vector>
#include <map>
#include <functional>

#include "game/entities/entity.h"
#include "core/util/gameassert.h"
#include "core/util/randomrolls.h"
#include "core/event/eventpublisher.h"
#include "game/entities/behaviour/behaviourstrategy.h"
#include "game/application/applicationcontext.h"

struct TurnControllerEventData {
    int turnNumber;
    int currentParticipant;
};

class TurnController : public EventPublisher<TurnControllerEventData> {
public:
    enum Action {
        Move = 0,
        Attack,
        // Freeze,
        Count
    };

    // TODO: Consider making entities a map rather than a set
    typedef struct _participant {
        int id;
        bool isReady;
        bool isPlayer;
        std::vector<Entity*> entities;
        bool passNextTurn;
        std::map<Action, int> actions;
        bool hasRolledForActions;
        std::unique_ptr<BehaviourStrategy> behaviourStrategy;
    } Participant;

protected:
    ApplicationContext* context;
    bool initialised;

    int turnNumber;
    int currentParticipant;

    std::map<int, std::unique_ptr<Participant>> participants;
    std::vector<std::function<void(int, int)>> onNextTurnWorkers;
    std::function<void()> onAllParticipantsSet;

    void nextParticipantTurn(int id);

    virtual bool canProgressToNextTurn(int participantId) = 0;

public:
    TurnController();

    void initialise(ApplicationContext& context);
    virtual void update(int64_t timeSinceLastFrame, bool& quit);

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

    void reset(void);
    
    void passParticipant(int id);
    void setCurrentParticipant(int id);
    int getCurrentParticipant(void) const;

    void setActions(int participantId, const std::map<Action, int>& actions);
    std::map<Action, int> rollActions(int participantId);
    bool performMoveAction(
        Entity* entity, 
        const glm::ivec2& position,
        int shortStopSteps = 0
    );
    bool performAttackAction(
        Entity* entity, 
        Weapon* weapon,
        const glm::ivec2& target
    );

    void addOnNextTurnFunction(std::function<void(int, int)> onNextTurnFunc);
    void setOnAllParticipantsSetFunction(std::function<void()> onAllParticipantsSet);

    void allParticipantsSet(void);

    int getTurnNumber(void) const;
};