#pragma once

#include <vector>
#include <map>

#include "core/json.hpp"
#include "areaofeffect.h"
#include "game/application/turncontroller.h"

using json = nlohmann::json;

class AreaOfEffectPool {
private:
    typedef struct _aoeDefinition {
        std::string filename;
        std::string name;
        uint8_t textureId;
        float radius;
        int turns;
    } AoeDefinition;

    std::map<std::string, AoeDefinition> aoeDefinitions;

    std::vector<std::pair<int, std::shared_ptr<AreaOfEffect>>> aoeObjects;
    std::vector<std::pair<int, std::shared_ptr<AreaOfEffect>>> aoeObjectsForDeletion;

    std::shared_ptr<TurnController> turnController;
    std::shared_ptr<Grid> grid;

    void loadAoeDefinitions(void);

public:
    AreaOfEffectPool(
        const std::shared_ptr<TurnController>& turnController, 
        const std::shared_ptr<Grid>& grid
    );

    void add(const std::shared_ptr<AreaOfEffect>& areaOfEffect);
    void add(const std::string& name, const int& turnNumber, const glm::ivec2& position);

    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);
    void update(const uint32_t& timeSinceLastFrame);
};