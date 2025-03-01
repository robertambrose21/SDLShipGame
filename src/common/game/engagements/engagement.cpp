#include "engagement.h"

Engagement::Engagement(uint32_t id, const std::vector<Participant*>& participants) :
    id(id),
    participants(participants),
    turnNumber(0),
    currentParticipantIndex(participants[0]->getId())
{ }

Engagement::Engagement(const std::vector<Participant*>& participants) :
    Engagement(getNewId(), participants) 
{ }

void Engagement::nextTurn(void) {
    currentParticipantIndex = (currentParticipantIndex + 1) % participants.size();
    getCurrentParticipant()->nextTurn();

    // for(auto const& onNextTurnFunc : onNextTurnWorkers) {
    //     onNextTurnFunc(currentParticipantId, turnNumber);
    // }
    // context->getEffectController()->onNextTurn();

    turnNumber++;
    // context->getGrid()->nextTurn();
    // publish<TurnEventData>({ turnNumber, currentParticipantId });
}

void Engagement::addParticipant(Participant* participant) {
    if(contains(participants, participant)) {
        spdlog::warn("Participant with id {} already exists for engagement {}", participant->getId(), id);
        return;
    }

    participants.push_back(participant);
}

void Engagement::removeParticipant(int participantId) {
    std::erase_if(participants, [&](const auto& item) {
        return participantId == item->getId();
    });
}

uint32_t Engagement::getId(void) const {
    return id;
}

Participant* Engagement::getCurrentParticipant(void) {
    return participants[currentParticipantIndex];
}

int Engagement::getTurnNumber(void) const {
    return turnNumber;
}

const std::vector<Participant*>& Engagement::getParticipants(void) const {
    return participants;
}