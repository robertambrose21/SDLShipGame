#include "entitypool.h"

EntityPool::EntityPool(
    TurnController& turnController,
    WeaponController& weaponController
) :
    turnController(turnController),
    weaponController(weaponController)
{
    loadEntityDefinitions();
}

void EntityPool::loadEntityDefinitions(void) {
    std::string directory = "../assets/data/entities";

    for(const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::ifstream f(entry.path());
        json data = json::parse(f);

        EntityDefinition definition;
        definition.filename = entry.path();
        definition.name = data["name"].get<std::string>();
        definition.movesPerTurn = data["movesPerTurn"].get<int>();
        definition.hp = data["hp"].get<int>();

        auto const& textureData = data["texture"].get<json>();
        definition.textureId = textureData["id"].get<uint32_t>();
        auto const& colourData = textureData["colour"].get<json>();
        definition.r = colourData["r"].get<uint8_t>();
        definition.g = colourData["g"].get<uint8_t>();
        definition.b = colourData["b"].get<uint8_t>();
        definition.a = colourData["a"].get<uint8_t>();

        std::cout << "Loaded entity definition \"" << definition.name << "\"" << std::endl;

        entityDefinitions[definition.name] = definition;
    }

    game_assert(!entityDefinitions.empty());
}

void EntityPool::updateEntities(uint32_t timeSinceLastFrame, bool& quit) {
    synchronize();

    for(auto const& entityId : entitiesForDeletion) {
        auto entity = getEntity(entityId);
        turnController.getParticipant(entity->getParticipantId())->entities.erase(entity);
        entities.erase(entityId);
        // std::cout << "[" << entity->getName() << "#" << entity->getId() << "] died" << std::endl;
    }
    
    entitiesForDeletion.clear();

    for(auto& [entityId, entity] : entities) {
        updateEntity(entity.get(), timeSinceLastFrame, quit);
    }
}

void EntityPool::updateEntity(Entity* entity, uint32_t timeSinceLastFrame, bool& quit) {
    if(entity->getCurrentHP() <= 0) {
        entitiesForDeletion.insert(entity->getId());
        return;
    }

    entity->update(timeSinceLastFrame, quit);
}

void EntityPool::drawEntities(GraphicsContext& graphicsContext) {
    for(auto& [entityId, entity] : entities) {
        entity->draw(graphicsContext);
    }
}

void EntityPool::synchronize() {
    if(pendingUpdates.empty()) {
        return;
    }

    std::map<int, Entity*> updatedEntities;

    for(auto const& update : pendingUpdates) {
        // std::cout << "Got game state update: " << std::endl;

        for(int i = 0; i < update.numEntities; i++) {
            auto const& entityUpdate = update.entities[i];

            if(!entities.contains(entityUpdate.id)) {
                auto const& entity = addEntity(entityUpdate.name, entityUpdate.id);
                turnController.addEntityToParticipant(entityUpdate.participantId, entity);   
            }

            auto& existing = entities[entityUpdate.id];

            // Weapons
            for(int j = 0; j < entityUpdate.numWeapons; j++) {
                auto const& weaponUpdate = entityUpdate.weaponUpdates[j];
                
                if(!existing->hasWeapon(weaponUpdate.id)) {
                    existing->addWeapon(weaponController.createWeapon(weaponUpdate.id, weaponUpdate.name, existing.get()));
                }
            }

            EntityStateUpdate::deserialize(entityUpdate, existing.get());

            if(entityUpdate.currentHP <= 0) {
                entitiesForDeletion.insert(entityUpdate.id);
            } else {
                updatedEntities[entityUpdate.id] = entities[entityUpdate.id].get();
            }

            // std::cout << "Entity [" << update.entities[i].participantId << "] " << update.entities[i].name << "#" 
            //     << update.entities[i].id << "(" << update.entities[i].currentHP << "/" 
            //     << update.entities[i].totalHP << "):" << std::endl;
            // std::cout << "\tPosition: (" << update.entities[i].x << ", " <<  update.entities[i].y << ")" << std::endl;
            // std::cout << "\tMoves per turn: " << update.entities[i].movesPerTurn << std::endl;
            // std::cout << "\tMoves left: " << update.entities[i].movesLeft << std::endl;
        }
    }

    // Remove any entities which weren't present in the updates
    for(auto& [entityId, entity] : entities) {
        if(!updatedEntities.contains(entityId)) {
            entitiesForDeletion.insert(entityId);
        }
    }

    pendingUpdates.clear();
}

void EntityPool::addGameStateUpdate(const GameStateUpdate& update) {
    pendingUpdates.push_back(update);
}

Entity* EntityPool::addEntity(std::unique_ptr<Entity> entity) {
    game_assert(!entities.contains(entity->getId()));

    auto id = entity->getId();
    entities[id] = std::move(entity);
    return entities[id].get();
}

Entity* EntityPool::addEntity(const std::string& name, uint32_t id) {
    game_assert(entityDefinitions.contains(name));

    auto definition = entityDefinitions[name];
    auto entity = std::make_unique<Entity>(
        id,
        definition.name,
        EntityBaseStats {
            definition.movesPerTurn,
            definition.hp
        }
    );
    entity->setTextureId(definition.textureId);
    entity->setColour({
        definition.r,
        definition.g,
        definition.b,
        definition.a
    });
    entity->setSelectedTextureId(6);

    return addEntity(std::move(entity));
}

Entity* EntityPool::addEntity(const std::string& name) {
    return addEntity(name, getNewId());
}

std::vector<Entity*> EntityPool::getEntities(void) {
    std::vector<Entity*> vEntities;
    
    for(auto& [_, entity] : entities) {
        if(entity != nullptr) {
            vEntities.push_back(entity.get());
        }
    }

    return vEntities;
}

Entity* EntityPool::getEntity(uint32_t id) {
    game_assert(entities.contains(id));
    return entities[id].get();
}

bool EntityPool::hasEntity(uint32_t id) {
    return entities.contains(id);
}
