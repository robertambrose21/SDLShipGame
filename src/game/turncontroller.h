#pragma once

#include <set>

#include "entitypool.h"

class TurnController {
private:
    typedef struct _participant {
        int order;
        bool isPlayer;
        std::set<std::shared_ptr<Entity>> entities;
    } Participant;

    int turnNumber;
    int currentParticipant;

    std::vector<Participant> participants;

    bool canProgressToNextTurn(std::shared_ptr<Entity> entity);
    void nextParticipantTurn(void);

public:
    TurnController();

    void update(Uint32 timeSinceLastFrame);

    void addParticipant(std::set<std::shared_ptr<Entity>> entities, bool isPlayer);
    void reset(void);
    
    void passCurrentParticipant(void);

    int getTurnNumber(void) const;
};