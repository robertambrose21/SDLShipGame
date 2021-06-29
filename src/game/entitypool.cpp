#include "entitypool.h"

EntityPool::EntityPool() :
    isPlayersTurn(false)
{ }

void EntityPool::updateEntities(Uint32 timeSinceLastFrame, bool& quit) {
    player->update(timeSinceLastFrame, quit);

    bool nonPlayerNextTurn = !isPlayersTurn;

    for(auto entity : entities) {
        updateEntity(entity, timeSinceLastFrame, quit);
        nonPlayerNextTurn = nonPlayerNextTurn && canProgressToNextTurn(entity);
    }

    if(nonPlayerNextTurn || (isPlayersTurn && canProgressToNextTurn(player))) {
        nextTurn();
    }
}

void EntityPool::updateEntity(std::shared_ptr<Entity> entity, Uint32 timeSinceLastFrame, bool& quit) {
    if(entity->getStats().hp <= 0) {
        entities.erase(std::find(entities.begin(), entities.end(), entity));
        return;
    }

    entity->update(timeSinceLastFrame, quit);
}

void EntityPool::drawEntities(std::shared_ptr<SDL_Renderer> renderer) {
    player->draw(renderer);

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

void EntityPool::nextTurn(void) {
    if(player == nullptr) {
        return;
    }

    isPlayersTurn = entities.empty() || !isPlayersTurn;

    if(isPlayersTurn) {
        player->nextTurn();
    } else {
        for(auto entity : entities) {
            entity->nextTurn();
        }
    }
}

bool EntityPool::canProgressToNextTurn(std::shared_ptr<Entity> entity) {
    return entity->getMovesLeft() == 0;
}

void EntityPool::setPlayerEntity(std::shared_ptr<Entity> player) {
    this->player = player;
}
