#pragma once

#include <set>
#include <vector>
#include <map>
#include <functional>

#include "game/entities/entity.h"
#include "core/util/gameassert.h"

class TurnController {
public:
    // TODO: Consider making entities a map rather than a set
    typedef struct _participant {
        int id;
        bool isPlayer;
        std::set<std::shared_ptr<Entity>> entities;
        bool passNextTurn;
    } Participant;

private:
    int turnNumber;
    int currentParticipant;

    std::map<int, std::shared_ptr<Participant>> participants;
    std::vector<std::function<void(int, int)>> onNextTurnWorkers;
    std::function<void()> onAllParticipantsSet;

    bool canProgressToNextTurn(const std::shared_ptr<Entity>& entity);
    void nextParticipantTurn(const int& id);

public:
    TurnController();

    void update(const uint32_t& timeSinceLastFrame);

    std::shared_ptr<Participant> addParticipant(
        const int& id,
        const std::set<std::shared_ptr<Entity>>& entities, 
        const bool& isPlayer
    );
    void addEntityToParticipant(const int& participantId, const std::shared_ptr<Entity>& entity);
    std::shared_ptr<Participant> getParticipant(const int& id);
    const std::map<int, std::shared_ptr<Participant>>& getParticipants(void) const;

    void reset(void);
    
    void passParticipant(const int& id);
    void setCurrentParticipant(const int& id);
    int getCurrentParticipant(void) const;

    void addOnNextTurnFunction(std::function<void(const int&, const int&)> onNextTurnFunc);
    void setOnAllParticipantsSetFunction(std::function<void()> onAllParticipantsSet);

    void allParticipantsSet(void);

    int getTurnNumber(void) const;
};