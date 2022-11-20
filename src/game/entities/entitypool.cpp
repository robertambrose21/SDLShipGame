#include "entitypool.h"

EntityPool::EntityPool()
{ }

void EntityPool::updateEntities(uint32_t timeSinceLastFrame, bool& quit) {
    for(auto [entityId, entity] : entities) {
        updateEntity(entity, timeSinceLastFrame, quit);
    }

    for(auto entity : entitiesForDeletion) {
        entities.erase(entity->getId());
        std::cout << "[" << entity->getName() << "] died" << std::endl;
    }
    
    entitiesForDeletion.clear();
}

void EntityPool::updateEntity(std::shared_ptr<Entity> entity, const uint32_t& timeSinceLastFrame, bool& quit) {
    if(entity->getCurrentHP() <= 0) {
        entitiesForDeletion.insert(entity);
        return;
    }

    entity->update(timeSinceLastFrame, quit);
}

void EntityPool::drawEntities(std::shared_ptr<GraphicsContext> graphicsContext) {
    for(auto [entityId, entity] : entities) {
        entity->draw(graphicsContext);
    }
}

void EntityPool::synchronize(std::vector<GameStateUpdate> updates) {
    std::map<int, std::shared_ptr<Entity>> updatedEntities;

    for(auto update : updates) {
        for(int i = 0; i < update.numEntities; i++) {
            auto entityUpdate = update.entities[i];
            auto existing = entities.contains(entityUpdate.id) ? entities[entityUpdate.id] : nullptr;

            entities[entityUpdate.id] = EntityStateUpdate::deserialize(entityUpdate, existing);

            if(entityUpdate.currentHP <= 0) {
                entitiesForDeletion.insert(entities[entityUpdate.id]);
            } else {
                updatedEntities[entityUpdate.id] = entities[entityUpdate.id];
            }
        }
    }

    // Remove any entities which weren't present in the updates
    for(auto [entityId, entity] : entities) {
        if(!updatedEntities.contains(entityId)) {
            entitiesForDeletion.insert(entity);
        }
    }
}

std::shared_ptr<Entity> EntityPool::addEntity(std::shared_ptr<Entity> entity) {
    if(entities.contains(entity->getId())) {
        // TODO: Throw exception
        return nullptr;
    }

    entities[entity->getId()] = entity;
    return entity;
}

// std::shared_ptr<Entity> EntityPool::addEntity(const std::string& path) {
//     std::ifstream f(path);
//     json data = json::parse(f);

//     Player e(
//         data["name"].get<std::string>(), 
//         { data["movesPerTurn"].get<int>(), data["hp"].get<int>() }
//     );
// }

std::map<uint32_t, std::shared_ptr<Entity>> EntityPool::getEntities(void) {
    return entities;
}

std::shared_ptr<Entity> EntityPool::getEntity(const uint32_t& id) {
    if(!entities.contains(id)) {
        // TODO: Throw exception
        return nullptr;
    }

    return entities[id];
}
