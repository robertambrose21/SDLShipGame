#include "engagement.h"

Engagement::Engagement(uint32_t id, const std::vector<Participant*>& participants) :
    id(id),
    participants(participants),
    turnNumber(0),
    currentParticipantIndex(participants[0]->getId()),
    isFinished(false)
{ }

Engagement::Engagement(const std::vector<Participant*>& participants) :
    Engagement(getNewId(), participants) 
{ }

void Engagement::nextTurn(void) {
    if(isFinished) {
        spdlog::trace("Engagemnet {} is finished, not executing nextTurn", id);
        return;
    }

    currentParticipantIndex = (currentParticipantIndex + 1) % participants.size();
    getCurrentParticipant()->nextTurn();

    for(auto const& onNextTurnFunc : onNextTurnWorkers) {
        onNextTurnFunc(getCurrentParticipant()->getId(), turnNumber, id);
    }
    
    // context->getEffectController()->onNextTurn();

    turnNumber++;

    spdlog::trace(
        "Engagement {} next turn: {} - current participant is {}",
        id,
        turnNumber,
        getCurrentParticipant()->getId()
    );
    // context->getGrid()->nextTurn();
    // publish<TurnEventData>({ turnNumber, currentParticipantId });
}

void Engagement::addParticipant(Participant* participant) {
    if(isFinished) {
        spdlog::error("Cannot add participant {} to finished engagement {}", participant->getId(), id);
        return;
    }

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

void Engagement::addOnNextTurnWorker(std::function<void(int, int, uint32_t)> worker) {
    if(isFinished) {
        spdlog::error("Cannot add next turn worker to finished engagement {}", id);
        return;
    }

    onNextTurnWorkers.push_back(worker);
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

bool Engagement::getIsFinished(void) const {
    return isFinished;
}

void Engagement::setIsFinished(bool isFinished) {
    this->isFinished = isFinished;
}

bool Engagement::hasHostileParticipants(void) {
    for(auto participant : participants) {
        for(auto otherParticipant : participants) {
            if(participant->isHostile(otherParticipant)) {
                return true;
            }
        }
    }

    return false;
}
