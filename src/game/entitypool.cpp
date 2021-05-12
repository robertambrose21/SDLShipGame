#include "entitypool.h"

EntityPool::EntityPool()
{ }

void EntityPool::updateEntities(Uint32 timeSinceLastFrame, bool& quit) {
    for(auto entity : entities) {
        updateEntity(entity, timeSinceLastFrame, quit);
    }
}

void EntityPool::updateEntity(std::shared_ptr<Entity> entity, Uint32 timeSinceLastFrame, bool& quit) {
    if(entity->getStats().hp <= 0) {
        entities.erase(entity);
        return;
    }

    entity->update(timeSinceLastFrame, quit);
}

void EntityPool::drawEntities(std::shared_ptr<SDL_Renderer> renderer) {
    for(auto entity : entities) {
        entity->draw(renderer);
    }
}

std::shared_ptr<Entity> EntityPool::createEntity(std::shared_ptr<Entity> entity) {
    entities.insert(entity);
    return entity;
}

std::set<std::shared_ptr<Entity>> EntityPool::getEntities(void) {
    return entities;
}
