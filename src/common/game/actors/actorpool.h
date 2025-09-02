#pragma once

#include <set>
#include <map>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <string>
#include <optional>

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
    void update(int64_t timeSinceLastFrame, bool& quit);
    void addGameStateUpdate(const GameStateUpdate& update);

    entt::entity addActor(const std::string& name);
    entt::entity addActor(const std::string& name, uint32_t id);
    void removeActor(uint32_t id);
    
    std::optional<entt::entity> getByExternalId(ExternalId externalId) const;

    Position getPosition(uint32_t actorId) const;
    void setPosition(uint32_t actorId, const Position& position);
    std::optional<entt::entity> findClosestTarget(entt::entity attacker, int participantId);

    LootTable getLootTable(const std::string& actorName);

    std::vector<entt::entity> filterByTile(int x, int y, int excludedParticipantId = -1);
    std::vector<entt::entity> filterByTile(
        int x, 
        int y, 
        const std::vector<entt::entity>& actors, 
        int excludedParticipantId = -1
    );
    std::vector<entt::entity> filterByTiles(
        const std::vector<glm::ivec2>& tiles, 
        int excludedParticipantId = -1
    );
    std::vector<entt::entity> filterByTiles(
        const std::vector<entt::entity>& actors,
        const std::vector<glm::ivec2>& tiles, 
        int excludedParticipantId = -1
    );

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
    std::map<uint32_t, entt::entity> actorIdsToEntities; // TODO: delete me;
    std::unordered_map<ExternalId, entt::entity> actorByExternalId;

    std::map<uint8_t, ChunkedGameStateUpdate> pendingChunkedUpdates;

    ApplicationContext* context;
    bool initialised;

    void loadActorDefinitions(void);
    void synchronize(void);
    bool applyChunkedGameStateUpdate(const ChunkedGameStateUpdate& chunked);
};