#pragma once

#include <set>
#include <map>
#include <string>

#include "spdlog/spdlog.h"

class Factioned {
public:
    enum Alignment: int {
        FRIENDLY,
        NEUTRAL,
        HOSTILE
    };

    Factioned() = delete;
    Factioned(const std::string& faction);

    std::string getFaction(void) const;
    void setFaction(const std::string& faction);

    bool isHostile(Factioned* other);
    void addFaction(const std::string& faction, Alignment alignment);
    void changeAlignment(const std::string& faction, Alignment existingAlignment, Alignment newAlignment);

private:
    std::string faction;
    std::map<Alignment, std::set<std::string>> factions;

    std::string alignmentToString(Alignment alignment);
};