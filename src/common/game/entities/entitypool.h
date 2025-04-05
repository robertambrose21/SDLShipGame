#pragma once

#include <set>
#include <map>
#include <fstream>
#include <filesystem>
#include <string>

#include "core/json.hpp"
#include "entity.h"
#include "game/application/gamecontroller.h"
#include "game/weapons/weaponcontroller.h"
#include "game/items/itemcontroller.h"
#include "game/net/messages.h"
#include "core/event/eventpublisher.h"
#include "game/application/applicationcontext.h"
#include "game/items/loottable.h"

using json = nlohmann::json;

struct GameStateUpdate;

class EntityPool : public EventPublisher<EntityEventData, EntitySetPositionEventData, EntityUpdateStatsEventData> {
private:
    typedef struct _entityDefinition {
        std::string filename;
        std::string name;
        uint32_t textureId;
        uint8_t r, g, b, a;
        int movesPerTurn;
        int hp;
        int armour;
        LootTable lootTable;
    } EntityDefinition;

    typedef struct _chunkedGameStateUpdate {
        std::vector<GameStateUpdate> pendingUpdates;
        int numExpectedChunks;        
    } ChunkedGameStateUpdate;

    std::map<std::string, EntityDefinition> entityDefinitions;

    std::set<uint32_t> entitiesForDeletion;
    std::map<uint32_t, std::unique_ptr<Entity>> entities;

    std::map<uint8_t, ChunkedGameStateUpdate> pendingChunkedUpdates;

    ApplicationContext* context;
    bool initialised;

    void updateEntity(Entity* entity, int64_t timeSinceLastFrame, bool& quit);
    void loadEntityDefinitions(void);
    void synchronize(void);
    bool applyChunkedGameStateUpdate(const ChunkedGameStateUpdate& chunked);
    void killEntity(uint32_t entityId);

public:
    EntityPool();

    void initialise(ApplicationContext& context);

    void updateEntities(int64_t timeSinceLastFrame, bool& quit);

    void addGameStateUpdate(const GameStateUpdate& update);

    Entity* addEntity(std::unique_ptr<Entity> entity);
    Entity* addEntity(const std::string& name);
    Entity* addEntity(const std::string& name, uint32_t id);
    void removeEntity(uint32_t id);
    std::vector<Entity*> getEntities(void);
    Entity* getEntity(uint32_t id);
    bool hasEntity(uint32_t id);

    Entity* findClosestTarget(Entity* attacker, int participantId);

    LootTable getLootTable(const std::string& entityName);
};