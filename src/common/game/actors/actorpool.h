#pragma once

#include <set>
#include <map>
#include <fstream>
#include <filesystem>
#include <string>

#include "core/json.hpp"
#include "actor.h"
#include "game/application/gamecontroller.h"
#include "game/weapons/weaponcontroller.h"
#include "game/items/itemcontroller.h"
#include "game/net/messages.h"
#include "core/event/eventpublisher.h"
#include "game/application/applicationcontext.h"
#include "game/items/loottable.h"
#include "game/ecs/components/drawable.h"

using json = nlohmann::json;

struct GameStateUpdate;

class ActorPool : public EventPublisher<ActorEventData, ActorSetPositionEventData, ActorUpdateStatsEventData> {
public:
    ActorPool();

    void initialise(ApplicationContext& context);

    void updateActors(int64_t timeSinceLastFrame, bool& quit);

    void addGameStateUpdate(const GameStateUpdate& update);

    Actor* addActor(const std::string& name);
    Actor* addActor(const std::string& name, uint32_t id);
    void removeActor(uint32_t id);
    std::vector<Actor*> getActors(void);
    Actor* getActor(uint32_t id);
    bool hasActor(uint32_t id);

    Actor* findClosestTarget(Actor* attacker, int participantId);

    LootTable getLootTable(const std::string& actorName);

private:
    typedef struct _actorDefinition {
        std::string filename;
        std::string name;
        uint32_t textureId;
        uint8_t r, g, b, a;
        int movesPerTurn;
        int hp;
        int armour;
        LootTable lootTable;
    } ActorDefinition;

    typedef struct _chunkedGameStateUpdate {
        std::vector<GameStateUpdate> pendingUpdates;
        int numExpectedChunks;        
    } ChunkedGameStateUpdate;

    std::map<std::string, ActorDefinition> actorDefinitions;

    std::set<uint32_t> actorsForDeletion;
    std::map<uint32_t, entt::entity> actorIdsToEntities;

    std::map<uint8_t, ChunkedGameStateUpdate> pendingChunkedUpdates;

    ApplicationContext* context;
    bool initialised;

    void updateActor(Actor* actor, int64_t timeSinceLastFrame, bool& quit);
    void loadActorDefinitions(void);
    void synchronize(void);
    bool applyChunkedGameStateUpdate(const ChunkedGameStateUpdate& chunked);
    void killActor(uint32_t actorId);

    Actor* getActorByEntityId(entt::entity entityId) const;
};