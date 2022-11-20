#pragma once

#include <set>
#include <functional>

#include "game/entities/entitypool.h"

class TurnController {
public:
    // TODO: Consider making entities a map rather than a set
    typedef struct _participant {
        int id;
        bool isPlayer;
        std::set<std::shared_ptr<Entity>> entities;
    } Participant;

private:
    int turnNumber;
    int currentParticipant;

    std::vector<std::shared_ptr<Participant>> participants;
    std::vector<std::function<void(int, int)>> onNextTurnWorkers;

    bool canProgressToNextTurn(std::shared_ptr<Entity> entity);
    void nextParticipantTurn(void);

public:
    TurnController();

    void update(const uint32_t& timeSinceLastFrame);

    std::shared_ptr<Participant> addParticipant(
        std::set<std::shared_ptr<Entity>> entities, 
        bool isPlayer
    );
    std::shared_ptr<Participant> getParticipant(int id);
    void reset(void);
    
    // TODO: Figure out a way to stop other participants from passing other turns
    void passCurrentParticipant(void);

    void addOnNextTurnFunction(std::function<void(int, int)> onNextTurnFunc);

    int getTurnNumber(void) const;
};