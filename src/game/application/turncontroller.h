#pragma once

#include <set>
#include <vector>
#include <map>
#include <functional>

#include "game/entities/entity.h"
#include "core/util/gameassert.h"
#include "core/util/randomrolls.h"
#include "game/entities/behaviour/behaviourstrategy.h"

class TurnController {
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
        bool isPlayer;
        std::set<std::shared_ptr<Entity>> entities;
        bool passNextTurn;
        std::map<Action, int> actions;
        bool hasRolledForActions;
        std::shared_ptr<BehaviourStrategy> behaviourStrategy;
    } Participant;

private:
    int turnNumber;
    int currentParticipant;

    std::map<int, std::shared_ptr<Participant>> participants;
    std::vector<std::function<void(int, int)>> onNextTurnWorkers;
    std::function<void()> onAllParticipantsSet;

    bool canProgressToNextTurn(const std::shared_ptr<Participant>& participant);
    void nextParticipantTurn(int id);

public:
    TurnController();

    void update(uint32_t timeSinceLastFrame, bool& quit);

    std::shared_ptr<Participant> addParticipant(
        int id,
        bool isPlayer,
        const std::set<std::shared_ptr<Entity>>& entities, 
        const std::shared_ptr<BehaviourStrategy>& behaviourStrategy = nullptr
    );
    void addEntityToParticipant(int participantId, const std::shared_ptr<Entity>& entity);
    std::shared_ptr<Participant> getParticipant(int id);
    const std::map<int, std::shared_ptr<Participant>>& getParticipants(void) const;

    void reset(void);
    
    void passParticipant(int id);
    void setCurrentParticipant(int id);
    int getCurrentParticipant(void) const;

    void setActions(int participantId, const std::map<Action, int>& actions);
    std::map<Action, int> rollActions(int participantId);
    bool performMoveAction(
        const std::shared_ptr<Entity>& entity, 
        const glm::ivec2& position,
        int shortStopSteps = 0
    );
    bool performAttackAction(
        const std::shared_ptr<Entity>& entity, 
        const std::shared_ptr<Weapon>& weapon,
        const glm::ivec2& target
    );

    void addOnNextTurnFunction(std::function<void(int, int)> onNextTurnFunc);
    void setOnAllParticipantsSetFunction(std::function<void()> onAllParticipantsSet);

    void allParticipantsSet(void);

    int getTurnNumber(void) const;
};