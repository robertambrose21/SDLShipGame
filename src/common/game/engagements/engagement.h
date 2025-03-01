#pragma once

#include <vector>
#include <cstdint>

#include "spdlog/spdlog.h"
#include "game/participant/participant.h"

class Engagement {
public:
    bool operator<(const Engagement& other) const {
        return id < other.id;
    }

    Engagement(uint32_t id, const std::vector<Participant*>& participants);
    Engagement(const std::vector<Participant*>& participants);

    void nextTurn(void);
    void addParticipant(Participant* participant);
    void removeParticipant(int participantId);

    uint32_t getId(void) const;
    Participant* getCurrentParticipant(void);
    int getTurnNumber(void) const;
    const std::vector<Participant*>& getParticipants(void) const;

private:
    uint32_t id;
    std::vector<Participant*> participants;
    int currentParticipantIndex;
    int turnNumber;
};
