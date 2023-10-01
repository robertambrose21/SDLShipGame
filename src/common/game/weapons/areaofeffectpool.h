#pragma once

#include <vector>
#include <map>
#include <fstream>

#include "core/json.hpp"
#include "areaofeffect.h"
#include "game/application/turncontroller.h"
#include "core/event/eventpublisher.h"

using json = nlohmann::json;

class AreaOfEffectPool : public EventPublisher<AreaOfEffectEventData> {
private:
    typedef struct _aoeDefinition {
        std::string filename;
        std::string name;
        uint32_t textureId;
        float radius;
        int turns;
        int damagePerTurn;
    } AoeDefinition;

    std::map<std::string, AoeDefinition> aoeDefinitions;

    std::vector<std::pair<int, std::unique_ptr<AreaOfEffect>>> aoeObjects;
    std::vector<int> aoeObjectsForDeletion;

    TurnController& turnController;
    Grid& grid;

    void loadAoeDefinitions(void);

public:
    AreaOfEffectPool(
        TurnController& turnController, 
        Grid& grid
    );

    void add(const std::string& name, int ownerId, int turnNumber, const glm::ivec2& position);

    void update(int64_t timeSinceLastFrame);

    std::vector<AreaOfEffect*> getAoeEffects(void);
};