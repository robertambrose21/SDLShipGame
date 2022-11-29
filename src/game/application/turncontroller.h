#pragma once

#include <set>
#include <vector>
#include <map>
#include <functional>

#include "game/entities/entity.h"

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

    std::map<int, std::shared_ptr<Participant>> participants;
    std::vector<std::function<void(int, int)>> onNextTurnWorkers;

    bool canProgressToNextTurn(std::shared_ptr<Entity> entity);
    void nextParticipantTurn(void);

public:
    TurnController();

    void update(const uint32_t& timeSinceLastFrame);

    std::shared_ptr<Participant> addParticipant(
        const int& id,
        std::set<std::shared_ptr<Entity>> entities, 
        bool isPlayer
    );
    void addEntityToParticipant(const int& participantId, std::shared_ptr<Entity> entity);
    std::shared_ptr<Participant> getParticipant(const int& id);
    std::map<int, std::shared_ptr<Participant>> getParticipants(void);

    void reset(void);
    
    // TODO: Figure out a way to stop other participants from passing other turns
    void passCurrentParticipant(void);

    void addOnNextTurnFunction(std::function<void(int, int)> onNextTurnFunc);

    int getTurnNumber(void) const;
};