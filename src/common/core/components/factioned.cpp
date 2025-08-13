#include "factioned.h"

Factioned::Factioned(const std::string& faction) :
    faction(faction)
{ }

std::string Factioned::getFaction(void) const {
    return faction;
}

void Factioned::setFaction(const std::string& faction) {
    this->faction = faction;
}

bool Factioned::isHostile(Factioned* other) {
    return this != other && factions[HOSTILE].contains(other->getFaction());
}

void Factioned::addFaction(const std::string& faction, Alignment alignment) {
    factions[alignment].insert(faction);
}

void Factioned::changeAlignment(const std::string& faction, Alignment existingAlignment, Alignment newAlignment) {
    auto factionToChange = factions[existingAlignment].find(faction);

    if(factionToChange == factions[existingAlignment].end()) {
        spdlog::warn(
            "Cannot change alignment from '{}' to '{}' for faction {}, faction is not '{}'",
            existingAlignment,
            newAlignment,
            faction,
            existingAlignment
        );
        return;
    }
    
    factions[newAlignment].insert(*factionToChange);
    factions[existingAlignment].erase(factionToChange);
}

std::string Factioned::alignmentToString(Alignment alignment) {
    switch(alignment) {
        case FRIENDLY:
            return "Friendly";
        case NEUTRAL:
            return "Neutral";
        case HOSTILE:
            return "Hostile";
    }
}