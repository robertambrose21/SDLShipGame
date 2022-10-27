#include "entitypool.h"

EntityPool::EntityPool()
{ }

void EntityPool::updateEntities(Uint32 timeSinceLastFrame, bool& quit) {
    for(auto entity : entities) {
        updateEntity(entity, timeSinceLastFrame, quit);
    }

    for(auto entity : entitiesForDeletion) {
        entities.erase(std::find(entities.begin(), entities.end(), entity));
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
    for(auto entity : entities) {
        entity->draw(graphicsContext);
    }
}

std::shared_ptr<Entity> EntityPool::addEntity(std::shared_ptr<Entity> entity) {
    entities.insert(entity);
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

std::set<std::shared_ptr<Entity>> EntityPool::getEntities(void) {
    return entities;
}
