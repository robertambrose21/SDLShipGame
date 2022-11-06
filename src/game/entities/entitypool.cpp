#include "entitypool.h"

EntityPool::EntityPool()
{ }

void EntityPool::updateEntities(Uint32 timeSinceLastFrame, bool& quit) {
    for(auto [entityId, entity] : entities) {
        updateEntity(entity, timeSinceLastFrame, quit);
    }

    for(auto entity : entitiesForDeletion) {
        entities.erase(entity->getId());
        std::cout << "[" << entity->getName() << "] died" << std::endl;
    }
    
    entitiesForDeletion.clear();
}

void EntityPool::updateEntity(std::shared_ptr<Entity> entity, Uint32 timeSinceLastFrame, bool& quit) {
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
