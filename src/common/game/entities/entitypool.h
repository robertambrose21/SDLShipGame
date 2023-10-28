#pragma once

#include <set>
#include <map>
#include <fstream>
#include <filesystem>

#include "core/json.hpp"
#include "entity.h"
#include "entitystats.h"
#include "game/application/turncontroller.h"
#include "game/weapons/weaponcontroller.h"
#include "game/net/messages.h"
#include "core/event/eventpublisher.h"
#include "game/application/applicationcontext.h"
#include "game/items/loottable.h"

using json = nlohmann::json;

struct GameStateUpdate;

// TODO: game_assert intialisation stuff
class EntityPool : public EventPublisher<EntityEventData> {
private:
    typedef struct _entityDefinition {
        std::string filename;
        std::string name;
        uint32_t textureId;
        uint8_t r, g, b, a;
        int movesPerTurn;
        int hp;
        LootTable lootTable;
    } EntityDefinition;

    std::map<std::string, EntityDefinition> entityDefinitions;

    std::set<uint32_t> entitiesForDeletion;
    std::map<uint32_t, std::unique_ptr<Entity>> entities;

    std::vector<GameStateUpdate> pendingUpdates;

    ApplicationContext* context;
    bool initialised;

    void updateEntity(Entity* entity, int64_t timeSinceLastFrame, bool& quit);
    void loadEntityDefinitions(void);
    void synchronize(void);
    void killEntity(uint32_t entityId);

public:
    EntityPool();

    void initialise(ApplicationContext& context);

    void updateEntities(int64_t timeSinceLastFrame, bool& quit);

    void addGameStateUpdate(const GameStateUpdate& update);

    Entity* addEntity(std::unique_ptr<Entity> entity);
    Entity* addEntity(const std::string& name);
    Entity* addEntity(const std::string& name, uint32_t id);
    std::vector<Entity*> getEntities(void);
    Entity* getEntity(uint32_t id);
    bool hasEntity(uint32_t id);
};