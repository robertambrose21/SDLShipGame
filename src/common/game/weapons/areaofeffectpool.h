#pragma once

#include <vector>
#include <map>
#include <fstream>

#include "core/json.hpp"
#include "areaofeffect.h"
#include "game/application/turncontroller.h"
#include "core/event/eventpublisher.h"
#include "game/application/applicationcontext.h"

using json = nlohmann::json;

class AreaOfEffectPool : public EventPublisher<AreaOfEffectEventData> {
private:
    typedef struct _aoeDefinition {
        std::string filename;
        std::string name;
        uint32_t textureId;
        float radius;
        int turns;
        std::string damageSource;
        int power;
    } AoeDefinition;

    std::map<std::string, AoeDefinition> aoeDefinitions;

    std::vector<std::pair<int, std::unique_ptr<AreaOfEffect>>> aoeObjects;
    std::vector<int> aoeObjectsForDeletion;

    ApplicationContext* context;
    bool initialised;

    void loadAoeDefinitions(void);

public:
    AreaOfEffectPool();

    void initialise(ApplicationContext& context);

    void add(const std::string& name, int ownerId, int turnNumber, const glm::ivec2& position, bool isAnimationOnly);

    void update(int64_t timeSinceLastFrame);

    std::vector<AreaOfEffect*> getAoeEffects(void);
    AreaOfEffectStats getStatsFor(const std::string& key);
};