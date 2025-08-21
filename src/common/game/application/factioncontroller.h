#pragma once

#include <map>
#include <optional>

#include "spdlog/spdlog.h"
#include "core/util/idgenerator.h"
#include "core/ecs/components/factioned.h"
#include "game/participant/participant.h"
#include "core/event/eventpublisher.h"
#include "game/event/events.h"

class FactionController : public EventPublisher<
    CreateFactionEventData,
    SetFactionEventData,
    AddFactionEventData,
    ChangeFactionAlignmentEventData
> {
public:
    static const std::string UnalignedFaction;

    FactionController();

    void createFaction(const std::string& name);
    void createFaction(uint32_t id, const std::string& name);

    std::optional<Factioned::Faction> getFaction(uint32_t id) const;
    std::optional<Factioned::Faction> getFactionByName(const std::string& name) const;
    const std::map<uint32_t, Factioned::Faction>& getAllFactions(void) const;
    std::vector<Factioned::Faction> getAlignedFactions(void) const;
    uint32_t getUnalignedFactionId(void) const;

    void setParticipantFaction(Participant* participant, uint32_t factionId);
    void addFactionAlignment(
        Participant* participant, 
        uint32_t factionId, 
        Factioned::Faction::Alignment alignment
    );
    void changeAlignmentToFaction(
        Participant* participant, 
        uint32_t factionId, 
        Factioned::Faction::Alignment existingAlignment, 
        Factioned::Faction::Alignment newAlignment
    );

private:
    uint32_t unalignedFactionId;
    std::map<uint32_t, Factioned::Faction> factions;
};