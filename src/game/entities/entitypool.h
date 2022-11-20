#pragma once

#include <set>
#include <map>
#include <fstream>
#include <filesystem>

#include "core/json.hpp"
#include "entity.h"
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

    void updateEntity(std::shared_ptr<Entity> entity, const uint32_t& timeSinceLastFrame, bool& quit);
    void loadEntityDefinitions(void);

public:
    EntityPool();

    void updateEntities(const uint32_t& timeSinceLastFrame, bool& quit);
    void drawEntities(std::shared_ptr<GraphicsContext> graphicsContext);

    void synchronize(std::vector<GameStateUpdate> updates);

    std::shared_ptr<Entity> addEntity(std::shared_ptr<Entity> entity);
    std::shared_ptr<Entity> addEntity(const std::string& name);
    std::map<uint32_t, std::shared_ptr<Entity>> getEntities(void);
    std::shared_ptr<Entity> getEntity(const uint32_t& id);
};