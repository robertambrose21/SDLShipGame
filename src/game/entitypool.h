#pragma once

#include <set>

#include "entity.h"

class EntityPool {
private:
    std::set<std::shared_ptr<Entity>> entities;
    std::shared_ptr<Entity> player;
    bool isPlayersTurn;

    void updateEntity(std::shared_ptr<Entity> entity, Uint32 timeSinceLastFrame, bool& quit);

    bool canProgressToNextTurn(std::shared_ptr<Entity> entity);

public:
    EntityPool();

    void updateEntities(Uint32 timeSinceLastFrame, bool& quit);
    void drawEntities(std::shared_ptr<SDL_Renderer> renderer);

    std::shared_ptr<Entity> createEntity(std::shared_ptr<Entity> entity);
    std::set<std::shared_ptr<Entity>> getEntities(void);
    std::set<std::shared_ptr<Entity>> getEntitiesOnTile(const int& x, const int& y);

    void setPlayerEntity(std::shared_ptr<Entity> player);

    void nextTurn(void);
};