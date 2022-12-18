#pragma once

#include <set>
#include <map>
#include <fstream>
#include <filesystem>

#include "core/json.hpp"
#include "entity.h"
#include "game/application/turncontroller.h"
#include "game/weapons/weaponcontroller.h"
#include "game/net/messages.h"

using json = nlohmann::json;

struct GameStateUpdate;

class EntityPool {
private:
    typedef struct _entityDefinition {
        std::string filename;
        std::string name;
        uint32_t textureId;
        int movesPerTurn;
        int hp;
    } EntityDefinition;

    std::map<std::string, EntityDefinition> entityDefinitions;

    std::set<std::shared_ptr<Entity>> entitiesForDeletion;
    std::map<uint32_t, std::shared_ptr<Entity>> entities;

    std::vector<GameStateUpdate> pendingUpdates;

    std::shared_ptr<TurnController> turnController;
    std::shared_ptr<WeaponController> weaponController;

    void updateEntity(const std::shared_ptr<Entity>& entity, uint32_t timeSinceLastFrame, bool& quit);
    void loadEntityDefinitions(void);
    void synchronize(void);

public:
    EntityPool(
        const std::shared_ptr<TurnController>& turnController,
        const std::shared_ptr<WeaponController>& weaponController
    );

    void updateEntities(uint32_t timeSinceLastFrame, bool& quit);
    void drawEntities(const std::shared_ptr<GraphicsContext>& graphicsContext);

    void addGameStateUpdate(const GameStateUpdate& update);

    std::shared_ptr<Entity> addEntity(const std::shared_ptr<Entity>& entity);
    std::shared_ptr<Entity> addEntity(const std::string& name);
    std::shared_ptr<Entity> addEntity(const std::string& name, uint32_t id);
    const std::map<uint32_t, std::shared_ptr<Entity>>& getEntities(void) const;
    std::shared_ptr<Entity> getEntity(uint32_t id);
    bool hasEntity(uint32_t id);
};