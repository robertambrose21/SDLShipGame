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
        uint8_t textureId;
        int movesPerTurn;
        int hp;
    } EntityDefinition;

    std::map<std::string, EntityDefinition> entityDefinitions;

    std::set<std::shared_ptr<Entity>> entitiesForDeletion;
    std::map<uint32_t, std::shared_ptr<Entity>> entities;

    std::vector<GameStateUpdate> pendingUpdates;

    std::shared_ptr<TurnController> turnController;
    std::shared_ptr<WeaponController> weaponController;

    void updateEntity(std::shared_ptr<Entity> entity, const uint32_t& timeSinceLastFrame, bool& quit);
    void loadEntityDefinitions(void);
    void synchronize(void);

public:
    EntityPool(
        std::shared_ptr<TurnController> turnController,
        std::shared_ptr<WeaponController> weaponController
    );

    void updateEntities(const uint32_t& timeSinceLastFrame, bool& quit);
    void drawEntities(std::shared_ptr<GraphicsContext> graphicsContext);

    void addGameStateUpdate(const GameStateUpdate& update);

    std::shared_ptr<Entity> addEntity(std::shared_ptr<Entity> entity);
    std::shared_ptr<Entity> addEntity(const std::string& name);
    std::shared_ptr<Entity> addEntity(const std::string& name, const uint32_t& id);
    std::map<uint32_t, std::shared_ptr<Entity>> getEntities(void);
    std::shared_ptr<Entity> getEntity(const uint32_t& id);
    bool hasEntity(const uint32_t& id);
};