#include "factioned.h"

Factioned::Factioned(uint32_t factionId) :
    factionId(factionId)
{ }

uint32_t Factioned::getFactionId(void) const {
    return factionId;
}

void Factioned::setFaction(uint32_t factionId) {
    this->factionId = factionId;
}

bool Factioned::isHostile(Factioned* other) {
    return this != other && alignments[Faction::HOSTILE].contains(other->getFactionId());
}

void Factioned::addFaction(uint32_t factionId, Faction::Alignment alignment) {
    alignments[alignment].insert(factionId);
}

void Factioned::changeAlignment(uint32_t factionId, Faction::Alignment existingAlignment, Faction::Alignment newAlignment) {
    auto factionToChange = alignments[existingAlignment].find(factionId);

    if(factionToChange == alignments[existingAlignment].end()) {
        spdlog::warn(
            "Cannot change alignment from '{}' to '{}' for faction {}, faction is not '{}'",
            alignmentToString(existingAlignment),
            alignmentToString(newAlignment),
            factionId,
            alignmentToString(existingAlignment)
        );
        return;
    }
    
    alignments[newAlignment].insert(*factionToChange);
    alignments[existingAlignment].erase(factionToChange);
}

std::string Factioned::alignmentToString(Faction::Alignment alignment) {
    switch(alignment) {
        case Faction::FRIENDLY:
            return "Friendly";
        case Faction::NEUTRAL:
            return "Neutral";
        case Faction::HOSTILE:
            return "Hostile";
        default:
            return "Unknown";
    }
}