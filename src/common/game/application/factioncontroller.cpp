#include "factioncontroller.h"

const std::string FactionController::UnalignedFaction = "Unaligned";

FactionController::FactionController() {
    createFaction(UnalignedFaction);
    unalignedFactionId = getFactionByName(UnalignedFaction)->id;
}

void FactionController::createFaction(const std::string& name) {
    createFaction(getNewId(), name);
}

void FactionController::createFaction(uint32_t id, const std::string& name) {
    if(getFactionByName(name) != std::nullopt) {
        spdlog::warn("Faction with name '{}' already exists", name);
        return;
    }

    factions[id] = { id, name };
    publish<CreateFactionEventData>({ id, name });
}

std::optional<Factioned::Faction> FactionController::getFaction(uint32_t id) const {
    if(!factions.contains(id)) {
        return std::nullopt;
    }

    return factions.at(id);
}

std::optional<Factioned::Faction> FactionController::getFactionByName(const std::string& name) const {
    auto it = std::find_if(factions.begin(), factions.end(), [&](const auto& kv) { return kv.second.name == name; });

    if(it == factions.end()) {
        return std::nullopt;
    }

    return it->second;
}

const std::map<uint32_t, Factioned::Faction>& FactionController::getAllFactions(void) const {
    return factions;
}

std::vector<Factioned::Faction> FactionController::getAlignedFactions(void) const {
    std::vector<Factioned::Faction> alignedFactions;

    for(const auto& [factionId, faction] : factions) {
        if(factionId == getUnalignedFactionId()) {
            continue;
        }

        alignedFactions.push_back(faction);
    }

    return alignedFactions;
}

uint32_t FactionController::getUnalignedFactionId(void) const {
    return unalignedFactionId;
}

void FactionController::setParticipantFaction(Participant* participant, uint32_t factionId) {
    if(getFaction(factionId) == std::nullopt) {
        spdlog::warn("Cannot set faction with id {}, to participant. Faction does not exist", factionId);
        return;
    }

    participant->setFaction(factionId);
    publish<SetFactionEventData>({ participant->getId(), factionId, });
}

void FactionController::addFactionAlignment(
    Participant* participant, 
    uint32_t factionId, 
    Factioned::Faction::Alignment alignment
) {
    if(getFaction(factionId) == std::nullopt) {
        spdlog::warn("Cannot add faction with id {}, to participant. Faction does not exist", factionId);
        return;
    }

    participant->addFaction(factionId, alignment);
    publish<AddFactionEventData>({ participant->getId(), factionId, alignment });
}

void FactionController::changeAlignmentToFaction(
    Participant* participant, 
    uint32_t factionId, 
    Factioned::Faction::Alignment existingAlignment, 
    Factioned::Faction::Alignment newAlignment
) {
    if(getFaction(factionId) == std::nullopt) {
        spdlog::warn("Cannot change alginment for faction with id {}, to participant. Faction does not exist", factionId);
        return;
    }

    participant->changeAlignment(factionId, existingAlignment, newAlignment);
    publish<ChangeFactionAlignmentEventData>({ participant->getId(), factionId, existingAlignment, newAlignment });
}