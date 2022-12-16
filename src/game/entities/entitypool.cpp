#include "entitypool.h"

EntityPool::EntityPool(
    const std::shared_ptr<TurnController>& turnController,
    const std::shared_ptr<WeaponController>& weaponController
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
        definition.textureId = data["textureId"].get<uint8_t>();
        definition.movesPerTurn = data["movesPerTurn"].get<int>();
        definition.hp = data["hp"].get<int>();

        std::cout << "Loaded entity definition \"" << definition.name << "\"" << std::endl;

        entityDefinitions[definition.name] = definition;
    }

    game_assert(!entityDefinitions.empty());
}

void EntityPool::updateEntities(const uint32_t& timeSinceLastFrame, bool& quit) {
    synchronize();

    for(auto [entityId, entity] : entities) {
        updateEntity(entity, timeSinceLastFrame, quit);
    }

    for(auto const& entity : entitiesForDeletion) {
        entities.erase(entity->getId());
        std::cout << "[" << entity->getName() << "#" << entity->getId() << "] died" << std::endl;
    }
    
    entitiesForDeletion.clear();
}

void EntityPool::updateEntity(const std::shared_ptr<Entity>& entity, const uint32_t& timeSinceLastFrame, bool& quit) {
    if(entity->getCurrentHP() <= 0) {
        entitiesForDeletion.insert(entity);
        return;
    }

    entity->update(timeSinceLastFrame, quit);
}

void EntityPool::drawEntities(const std::shared_ptr<GraphicsContext>& graphicsContext) {
    for(auto [entityId, entity] : entities) {
        entity->draw(graphicsContext);
    }
}

void EntityPool::synchronize() {
    if(pendingUpdates.empty()) {
        return;
    }

    std::map<int, std::shared_ptr<Entity>> updatedEntities;

    for(auto const& update : pendingUpdates) {
        // std::cout << "Got game state update: " << std::endl;

        for(int i = 0; i < update.numEntities; i++) {
            auto const& entityUpdate = update.entities[i];

            if(!entities.contains(entityUpdate.id)) {
                auto const& entity = addEntity(entityUpdate.name, entityUpdate.id);
                turnController->addEntityToParticipant(entityUpdate.participantId, entity);   
            }

            auto const& existing = entities[entityUpdate.id];

            // Weapons
            for(int j = 0; j < entityUpdate.numWeapons; j++) {
                auto const& weaponUpdate = entityUpdate.weaponUpdates[j];
                
                if(!existing->hasWeapon(weaponUpdate.id)) {
                    existing->addWeapon(weaponController->createWeapon(weaponUpdate.id, weaponUpdate.name, existing));
                }
            }

            entities[entityUpdate.id] = EntityStateUpdate::deserialize(entityUpdate, existing);

            if(entityUpdate.currentHP <= 0) {
                entitiesForDeletion.insert(entities[entityUpdate.id]);
            } else {
                updatedEntities[entityUpdate.id] = entities[entityUpdate.id];
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
    for(auto [entityId, entity] : entities) {
        if(!updatedEntities.contains(entityId)) {
            entitiesForDeletion.insert(entity);
        }
    }

    pendingUpdates.clear();
}

void EntityPool::addGameStateUpdate(const GameStateUpdate& update) {
    pendingUpdates.push_back(update);
}

std::shared_ptr<Entity> EntityPool::addEntity(const std::shared_ptr<Entity>& entity) {
    game_assert(!entities.contains(entity->getId()));
    entities[entity->getId()] = entity;
    return entity;
}

std::shared_ptr<Entity> EntityPool::addEntity(const std::string& name, const uint32_t& id) {
    game_assert(entityDefinitions.contains(name));

    auto definition = entityDefinitions[name];
    auto entity = std::make_shared<Entity>(
        id,
        definition.name,
        Entity::Stats {
            definition.movesPerTurn,
            definition.hp
        }
    );
    entity->setTextureId(definition.textureId);
    entity->setSelectedTextureId(6);

    return addEntity(entity);
}

std::shared_ptr<Entity> EntityPool::addEntity(const std::string& name) {
    return addEntity(name, getNewId());
}

const std::map<uint32_t, std::shared_ptr<Entity>>& EntityPool::getEntities(void) const {
    return entities;
}

std::shared_ptr<Entity> EntityPool::getEntity(const uint32_t& id) {
    game_assert(entities.contains(id));
    return entities[id];
}

bool EntityPool::hasEntity(const uint32_t& id) {
    return entities.contains(id);
}
