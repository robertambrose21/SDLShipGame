#pragma once

#include <set>

#include "game/entities/entitypool.h"

class TurnController {
public:
     typedef struct _participant {
        int id;
        bool isPlayer;
        std::set<std::shared_ptr<Entity>> entities;
    } Participant;

private:
    int turnNumber;
    int currentParticipant;

    std::vector<std::shared_ptr<Participant>> participants;

    bool canProgressToNextTurn(std::shared_ptr<Entity> entity);
    void nextParticipantTurn(void);

public:
    TurnController();

    void update(Uint32 timeSinceLastFrame);

    std::shared_ptr<Participant> addParticipant(
        std::set<std::shared_ptr<Entity>> entities, 
        bool isPlayer
    );
    std::shared_ptr<Participant> getParticipant(int id);
    void reset(void);
    
    // TODO: Figure out a way to stop other participants from passing other turns
    void passCurrentParticipant(void);

    int getTurnNumber(void) const;
};