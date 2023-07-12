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
        uint32_t textureId;
        float radius;
        int turns;
        int damagePerTurn;
    } AoeDefinition;

    std::map<std::string, AoeDefinition> aoeDefinitions;

    std::vector<std::pair<int, std::unique_ptr<AreaOfEffect>>> aoeObjects;
    std::vector<int> aoeObjectsForDeletion;

    std::shared_ptr<TurnController> turnController;
    std::shared_ptr<Grid> grid;

    void loadAoeDefinitions(void);

public:
    AreaOfEffectPool(
        const std::shared_ptr<TurnController>& turnController, 
        const std::shared_ptr<Grid>& grid
    );

    void add(const std::string& name, int ownerId, int turnNumber, const glm::ivec2& position);

    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);
    void update(uint32_t timeSinceLastFrame);
};