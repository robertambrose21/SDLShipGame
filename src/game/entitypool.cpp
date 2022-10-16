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

void EntityPool::drawEntities(std::shared_ptr<SDL_Renderer> renderer) {
    for(auto entity : entities) {
        entity->draw(renderer);
    }
}

std::shared_ptr<Entity> EntityPool::addEntity(std::shared_ptr<Entity> entity) {
    entities.insert(entity);
    return entity;
}

std::set<std::shared_ptr<Entity>> EntityPool::getEntities(void) {
    return entities;
}
