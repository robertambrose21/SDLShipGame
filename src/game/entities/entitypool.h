#pragma once

#include <set>
#include <map>
#include <fstream>

#include "core/json.hpp"
#include "entity.h"
#include "enemy.h"
#include "game/net/messages.h"
#include "player/player.h"

using json = nlohmann::json;

struct GameStateUpdate;

class EntityPool {
private:
    std::set<std::shared_ptr<Entity>> entitiesForDeletion;
    std::map<uint32_t, std::shared_ptr<Entity>> entities;

    void updateEntity(std::shared_ptr<Entity> entity, Uint32 timeSinceLastFrame, bool& quit);

public:
    EntityPool();

    void updateEntities(Uint32 timeSinceLastFrame, bool& quit);
    void drawEntities(std::shared_ptr<GraphicsContext> graphicsContext);

    void synchronize(std::vector<GameStateUpdate> updates);

    std::shared_ptr<Entity> addEntity(std::shared_ptr<Entity> entity);
    // std::shared_ptr<Entity> addEntity(const std::string& path);
    std::map<uint32_t, std::shared_ptr<Entity>> getEntities(void);
    std::shared_ptr<Entity> getEntity(const uint32_t& id);
};