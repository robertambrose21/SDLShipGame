#pragma once

#include <vector>
#include <map>
#include <fstream>

#include "core/json.hpp"
#include "areaofeffect.h"
#include "game/application/gamecontroller.h"
#include "core/event/eventpublisher.h"
#include "game/application/applicationcontext.h"

using json = nlohmann::json;

class AreaOfEffectPool : public EventPublisher<AreaOfEffectEventData> {
public:
    AreaOfEffectPool();

    void initialise(ApplicationContext& context);

    void add(const std::string& name, int ownerId, const glm::ivec2& position, bool isAnimationOnly);

    void update(int64_t timeSinceLastFrame);

    const std::map<uint32_t, std::vector<std::unique_ptr<AreaOfEffect>>>& getEngagementAoEs(void) const;
    const std::vector<std::unique_ptr<AreaOfEffect>>& getAdhocAoEs(void) const;
    Stats::AoEStats getStatsFor(const std::string& key);

private:
    typedef struct _aoeDefinition {
        std::string filename;
        std::string name;
        uint32_t textureId;
        float radius;
        uint8_t turns;
        std::string damageSource;
        int power;
    } AoeDefinition;

    std::map<std::string, AoeDefinition> aoeDefinitions;

    std::map<uint32_t, std::vector<std::unique_ptr<AreaOfEffect>>> engagementAoEs;
    std::vector<std::unique_ptr<AreaOfEffect>> adhocAoEs;
    std::vector<int> aoeObjectsForDeletion;

    ApplicationContext* context;
    bool initialised;

    void loadAoeDefinitions(void);
};
