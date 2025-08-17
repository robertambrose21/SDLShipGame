#pragma once

#include <set>
#include <map>
#include <string>

#include "spdlog/spdlog.h"

class Factioned {
public:
    // TODO: Make this an "entity" - move to "entities" module
    typedef struct _faction {
         enum Alignment: int {
            FRIENDLY,
            NEUTRAL,
            HOSTILE
        };

        uint32_t id;
        std::string name;

        bool operator<(const _faction& other) const {
            return id < other.id;
        }
    } Faction;

    Factioned() = delete;
    Factioned(uint32_t factionId);

    uint32_t getFactionId(void) const;
    void setFaction(uint32_t factionId);

    bool isHostile(Factioned* other);
    void addFaction(const uint32_t factionId, Faction::Alignment alignment);
    void changeAlignment(uint32_t factionId, Faction::Alignment existingAlignment, Faction::Alignment newAlignment);

private:
    uint32_t factionId;
    std::map<Faction::Alignment, std::set<uint32_t>> alignments;

    std::string alignmentToString(Faction::Alignment alignment);
};