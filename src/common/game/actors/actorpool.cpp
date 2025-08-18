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

void ActorPool::updateActors(int64_t timeSinceLastFrame, bool& quit) {
    game_assert(initialised);

    synchronize();

    for(auto const& actorId : actorsForDeletion) {
        killActor(actorId);
    }
    
    actorsForDeletion.clear();

    for(auto& [actorId, actor] : actors) {
        updateActor(actor.get(), timeSinceLastFrame, quit);
    }
}

void ActorPool::updateActor(Actor* actor, int64_t timeSinceLastFrame, bool& quit) {
    game_assert(initialised);

    if(actor->getCurrentHP() <= 0) {
        actorsForDeletion.insert(actor->getId());
        return;
    }

    actor->update(timeSinceLastFrame, quit);
}

void ActorPool::killActor(uint32_t actorId) {
    auto gameController = context->getGameController();
    auto actor = getActor(actorId);

    // Remove actor from participant
    gameController->getParticipant(actor->getParticipantId())->removeActor(actor);

    // Remove visibility of actor from participants (and prevent a seg-fault)
    for(auto participant : gameController->getParticipants()) {
        participant->removeVisibleActor(actor);
    }

    publish<ActorEventData>({ actor, "Death" });

    actors.erase(actorId);
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

            if(!actors.contains(actorUpdate.id)) {
                auto const& actor = addActor(actorUpdate.name, actorUpdate.id);
                context->getGameController()->addActorToParticipant(actorUpdate.participantId, actor);   
            }

            auto& existing = actors[actorUpdate.id];

            // Weapons
            for(int j = 0; j < actorUpdate.numWeapons; j++) {
                auto const& weaponUpdate = actorUpdate.weaponUpdates[j];
                auto weaponId = UUID::fromBytes(weaponUpdate.idBytes);
                
                if(!existing->hasWeapon(weaponId)) {
                    spdlog::trace("Syncing weapon {} to actor {}", weaponId.getString(), existing->getId());
                    auto weapon = context->getWeaponController()->createWeapon(weaponId, weaponUpdate.name, existing.get());
                    
                    if(weapon->getItem() != nullptr && weaponUpdate.hasItem) {
                        weapon->getItem()->setId(weaponUpdate.itemId);
                    }

                    existing->addWeapon(std::move(weapon));
                }
            }

            ActorStateUpdate::deserialize(actorUpdate, existing.get());

            if(actorUpdate.currentHP <= 0) {
                actorsForDeletion.insert(actorUpdate.id);
            } else {
                updatedActors[actorUpdate.id] = actors[actorUpdate.id].get();
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
    for(auto& [actorId, actor] : actors) {
        if(!updatedActors.contains(actorId)) {
            actorsForDeletion.insert(actorId);
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

Actor* ActorPool::addActor(std::unique_ptr<Actor> actor) {
    game_assert(initialised);
    game_assert(!actors.contains(actor->getId()));

    auto id = actor->getId();
    actors[id] = std::move(actor);
    return actors[id].get();
}

Actor* ActorPool::addActor(const std::string& name, uint32_t id) {
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
    
    auto actor = std::make_unique<Actor>(
        context->getGrid(),
        id,
        *this,
        definition.name,
        stats
    );
    actor->setTextureId(definition.textureId);
    actor->setColour({
        definition.r,
        definition.g,
        definition.b,
        definition.a
    });
    actor->setSelectedTextureId(6);

    return addActor(std::move(actor));
}

Actor* ActorPool::addActor(const std::string& name) {
    game_assert(initialised);
    return addActor(name, getNewId());
}

void ActorPool::removeActor(uint32_t id) {
    auto participant = context->getGameController()->getParticipant(actors[id]->getParticipantId());
    
    participant->removeActor(actors[id].get());
    actors.erase(id);
}

std::vector<Actor*> ActorPool::getActors(void) {
    game_assert(initialised);
    std::vector<Actor*> vActors;
    
    for(auto& [_, actor] : actors) {
        if(actor != nullptr) {
            vActors.push_back(actor.get());
        }
    }

    return vActors;
}

Actor* ActorPool::getActor(uint32_t id) {
    game_assert(initialised);
    game_assert(actors.contains(id));
    return actors[id].get();
}

bool ActorPool::hasActor(uint32_t id) {
    game_assert(initialised);
    return actors.contains(id);
}

Actor* ActorPool::findClosestTarget(Actor* attacker, int participantId) {
    Actor* closestActor = nullptr;
    auto shortestDistance = attacker->getDisengagementRange();
    
    for(auto& [_, actor] : actors) {
        if(actor->getParticipantId() == participantId) {
            continue;
        }

        auto distance = glm::distance(glm::vec2(attacker->getPosition()), glm::vec2(actor->getPosition()));

        if(distance < shortestDistance) {
            shortestDistance = distance;
            closestActor = actor.get();
        }
    }

    return closestActor;
}

LootTable ActorPool::getLootTable(const std::string& actorName) {
    return actorDefinitions[actorName].lootTable;
}
