#include "actorpool.h"

ActorPool::ActorPool() {
    loadActorDefinitions();
}

void ActorPool::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

void ActorPool::loadActorDefinitions(void) {
    // TODO: Change me to "actors"
    std::string directory = "../assets/data/entities";

    for(const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::ifstream f(entry.path());
        json data = json::parse(f);

        ActorDefinition definition;
        definition.filename = entry.path();
        definition.name = data["name"].get<std::string>();

        auto const& stats = data["stats"].get<json>();
        definition.movesPerTurn = stats["movesPerTurn"].get<int>();
        definition.hp = stats["hp"].get<int>();
        definition.armour = stats["armour"].get<int>();

        auto const& textureData = data["texture"].get<json>();
        definition.textureId = textureData["id"].get<uint32_t>();
        auto const& colourData = textureData["colour"].get<json>();
        definition.r = colourData["r"].get<uint8_t>();
        definition.g = colourData["g"].get<uint8_t>();
        definition.b = colourData["b"].get<uint8_t>();
        definition.a = colourData["a"].get<uint8_t>();

        std::vector<LootTableItem> lootTableItems;

        auto const& lootTableData = data["lootTable"].get<std::vector<json>>();
        for(auto const& lootTableItemsData : lootTableData) {
            LootTableItem lootTableItem;
            lootTableItem.totalWeights = 0.0;

            auto const& itemsData = lootTableItemsData["items"].get<std::vector<json>>();
            for(auto const& itemData : itemsData) {
                std::string name = itemData["name"].get<std::string>();
                double weight = 1.0;

                if(itemData.contains("weight")) {
                    weight = itemData["weight"].get<double>();
                }

                lootTableItem.items.push_back({ name, weight });
                lootTableItem.totalWeights += weight;
            }

            lootTableItem.percentChance = lootTableItemsData["chance"].get<uint8_t>();
            lootTableItems.push_back(lootTableItem);
        }

        definition.lootTable = LootTable(lootTableItems);

        std::cout << "Loaded actor definition \"" << definition.name << "\"" << std::endl;

        actorDefinitions[definition.name] = definition;
    }

    game_assert(!actorDefinitions.empty());
}

void ActorPool::update(int64_t timeSinceLastFrame, bool& quit) {
    game_assert(initialised);

    synchronize();
}

// TODO: Doing too much, break this up
bool ActorPool::applyChunkedGameStateUpdate(const ChunkedGameStateUpdate& chunked) {
    if(chunked.pendingUpdates.size() < chunked.numExpectedChunks) {
        return false; // Not ready
    }

    std::map<int, Actor*> updatedActors;

    for(auto const& update : chunked.pendingUpdates) {
        // std::cout << "Got game state update: " << std::endl;

        for(int i = 0; i < update.numActors; i++) {
            auto const& actorUpdate = update.actors[i];

            if(!actorByExternalId.contains(actorUpdate.id)) {
                auto newEntity = addActor(actorUpdate.name, actorUpdate.id);
                context->getGameController()->addActorToParticipant(actorUpdate.participantId, newEntity);   
            }

            auto entity = getByExternalId(actorUpdate.id);

            if(!entity.has_value()) {
                spdlog::warn("Cannot update non-existent actor {}", actorUpdate.id);
                continue;
            }

            auto& existing = context->getEntityRegistry().get<Actor>(entity.value());

            // Weapons
            for(int j = 0; j < actorUpdate.numWeapons; j++) {
                auto const& weaponUpdate = actorUpdate.weaponUpdates[j];
                auto weaponId = UUID::fromBytes(weaponUpdate.idBytes);
                
                if(!existing.hasWeapon(weaponId)) {
                    spdlog::trace("Syncing weapon {} to actor {}", weaponId.getString(), existing.getId());
                    auto weapon = context->getWeaponController()->createWeapon(weaponId, weaponUpdate.name, entity.value());
                    
                    if(weapon->getItem() != nullptr && weaponUpdate.hasItem) {
                        weapon->getItem()->setId(weaponUpdate.itemId);
                    }

                    existing.addWeapon(std::move(weapon));
                }
            }

            context->getActorController()->applyStats(entity.value());

            ActorStateUpdate::deserialize(context, actorUpdate, entity.value());

            if(actorUpdate.currentHP <= 0) {
                actorsForDeletion.insert(actorUpdate.id);
            } else {
                updatedActors[actorUpdate.id] = &existing;
            }

            // std::cout << "Actor [" << update.actors[i].participantId << "] " << update.actors[i].name << "#" 
            //     << update.actors[i].id << "(" << update.actors[i].currentHP << "/" 
            //     << update.actors[i].totalHP << "):" << std::endl;
            // std::cout << "\tPosition: (" << update.actors[i].x << ", " <<  update.actors[i].y << ")" << std::endl;
            // std::cout << "\tMoves per turn: " << update.actors[i].movesPerTurn << std::endl;
            // std::cout << "\tMoves left: " << update.actors[i].movesLeft << std::endl;
        }
    }

    // Remove any actors which weren't present in the updates
    for(auto [_, actor]: context->getEntityRegistry().view<Actor>().each()) {
         if(!updatedActors.contains(actor.getId())) {
            actorsForDeletion.insert(actor.getId());
        }
    }

    std::cout 
        << "Sync "
        << updatedActors.size() 
        << " updated " 
        << actorsForDeletion.size() 
        << " removed" 
        << std::endl;

    return true;
}

void ActorPool::synchronize() {
    game_assert(initialised);

    for(auto const& actorId : actorsForDeletion) {
        removeActor(actorId);
    }

    if(pendingChunkedUpdates.empty()) {
        return;
    }

    std::set<uint8_t> appliedChunks;

    for(auto [chunkId, chunked] : pendingChunkedUpdates) {
        if(applyChunkedGameStateUpdate(chunked)) {
            appliedChunks.insert(chunkId);
        }
    }

    std::erase_if(pendingChunkedUpdates, [&](const auto& item) {
        auto const& [chunkId, _] = item;
        return appliedChunks.contains(chunkId);
    });
}

void ActorPool::addGameStateUpdate(const GameStateUpdate& update) {
    game_assert(initialised);

    if(update.numExpectedChunks < 1) {
        std::cout 
            << "Error: [Chunk ID: "
            << update.chunkId
            << "] has numExpectedChunks < 1, discarding"
            << std::endl;
        return;
    }

    if(!pendingChunkedUpdates.contains(update.chunkId)) {
        pendingChunkedUpdates[update.chunkId] = { std::vector<GameStateUpdate>(), update.numExpectedChunks };
    }
    else if(pendingChunkedUpdates[update.chunkId].numExpectedChunks != update.numExpectedChunks) {
        std::cout 
            << "Warning: [Chunk ID: "
            << update.chunkId
            << "] changed numExpectedChunks from "
            << pendingChunkedUpdates[update.chunkId].numExpectedChunks
            << " to "
            << update.numExpectedChunks
            << std::endl;
    }

    pendingChunkedUpdates[update.chunkId].pendingUpdates.push_back(update);

    if(pendingChunkedUpdates[update.chunkId].pendingUpdates.size() > pendingChunkedUpdates[update.chunkId].numExpectedChunks) {
        std::cout 
            << "Warning: [Chunk ID: "
            << update.chunkId
            << "] expected " 
            << pendingChunkedUpdates[update.chunkId].numExpectedChunks
            << " GameStateUpdates but got "
            << pendingChunkedUpdates[update.chunkId].pendingUpdates.size()
            << std::endl;
    }
}

entt::entity ActorPool::addActor(const std::string& name, uint32_t id) {
    game_assert(initialised);
    game_assert(actorDefinitions.contains(name));

    auto definition = actorDefinitions[name];

    // TODO: Move applying stats to another class
    Stats::ActorStats stats;
    stats.hp = definition.hp;
    stats.totalHp = definition.hp;
    stats.movesPerTurn = definition.movesPerTurn;
    stats.movesLeft = definition.movesPerTurn;
    stats.armour = definition.armour;

    auto& registry = context->getEntityRegistry();

    auto entity = registry.create();
    registry.emplace<ExternalId>(entity, id);
    registry.emplace<Drawable>(
        entity, 
        definition.textureId, 
        UINT32_C(6),
        Colour { definition.r, definition.g, definition.b, definition.a}
    );
    registry.emplace<Position>(entity, glm::ivec2(0, 0));
    registry.emplace<PositionDirty>(entity);
    registry.emplace<Stats::ActorStats>(entity, stats);

    auto actor = &registry.emplace<Actor>(entity, context->getGrid(), id, definition.name, stats);

    actorByExternalId[id] = entity;

    return entity;
}

entt::entity ActorPool::addActor(const std::string& name) {
    game_assert(initialised);
    return addActor(name, getNewId());
}

void ActorPool::removeActor(uint32_t id) {
    auto gameController = context->getGameController();
    auto entity = getByExternalId(id).value();
    auto& actor = context->getEntityRegistry().get<Actor>(entity);

    auto participant = context->getGameController()->getParticipant(actor.getParticipantId());

    // Remove visibility of actor from participants (and prevent a seg-fault)
    for(auto participant : gameController->getParticipants()) {
        participant->removeVisibleActor(entity);
    }

    participant->removeActor(entity);

    context->getEntityRegistry().destroy(actorByExternalId[id]);
    actorByExternalId.erase(id);
}

std::optional<entt::entity> ActorPool::getByExternalId(ExternalId externalId) const {
    if(!actorByExternalId.contains(externalId)) {
        return std::nullopt;
    }

    return actorByExternalId.at(externalId);
}

std::optional<entt::entity> ActorPool::findClosestTarget(entt::entity attacker, int participantId) {
    std::optional<entt::entity> closestActor = std::nullopt;
    
    auto& attackerActor = context->getEntityRegistry().get<Actor>(attacker);
    auto const& attackerPosition = context->getEntityRegistry().get<Position>(attacker);

    auto shortestDistance = attackerActor.getDisengagementRange();
    
    for(auto [entity, actor, position]: context->getEntityRegistry().view<Actor, Position>().each()) {
        if(actor.getParticipantId() == participantId) {
            continue;
        }

        auto distance = glm::distance(glm::vec2(attackerPosition), glm::vec2(position));

        if(distance < shortestDistance) {
            shortestDistance = distance;
            closestActor = entity;
        }
    }

    return closestActor;
}

LootTable ActorPool::getLootTable(const std::string& actorName) {
    return actorDefinitions[actorName].lootTable;
}

std::vector<entt::entity> ActorPool::filterByTile(int x, int y, int excludedParticipantId) {
    std::vector<entt::entity> entities;

    for(auto [entity, actor, position]: context->getEntityRegistry().view<Actor, Position>().each()) {
        if(actor.getParticipantId() == excludedParticipantId) {
            continue;
        }

        if(position == glm::ivec2(x, y)) {
            entities.push_back(entity);
        }
    }

    return entities;
}

std::vector<entt::entity> ActorPool::filterByTile(
    int x, 
    int y, 
    const std::vector<entt::entity>& actors, 
    int excludedParticipantId
) {
    std::vector<entt::entity> entities;

    for(auto entity : actors) {
        auto [actor, position] = context->getEntityRegistry().try_get<Actor, Position>(entity);

        if(actor == nullptr) {
            spdlog::debug(
                "ActorPool#filterByTile: Cannot find actor for entity {}", 
                static_cast<entt::id_type>(entity)
            );
            continue;
        }

        if(position == nullptr) {
            spdlog::debug(
                "ActorPool#filterByTile: Cannot find position for entity {}", 
                static_cast<entt::id_type>(entity)
            );
            continue;
        }

        if(actor->getParticipantId() == excludedParticipantId) {
            continue;
        }

        if(*position == glm::ivec2(x, y)) {
            entities.push_back(entity);
        }
    }

    return entities;
}

std::vector<entt::entity> ActorPool::filterByTiles(
    const std::vector<glm::ivec2>& tiles, 
    int excludedParticipantId
) {
    std::vector<entt::entity> entities;

    for(auto const& tile : tiles) {
        auto entitiesByTile = filterByTile(tile.x, tile.y, excludedParticipantId);
        entities.insert(entities.end(), entitiesByTile.begin(), entitiesByTile.end());
    }

    return entities;
}

std::vector<entt::entity> ActorPool::filterByTiles(
    const std::vector<entt::entity>& actors,
    const std::vector<glm::ivec2>& tiles, 
    int excludedParticipantId
) {
    std::vector<entt::entity> entities;

    for(auto const& tile : tiles) {
        auto entitiesByTile = filterByTile(tile.x, tile.y, actors, excludedParticipantId);
        entities.insert(entities.end(), entitiesByTile.begin(), entitiesByTile.end());
    }

    return entities;
}
