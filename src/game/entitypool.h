#pragma once

#include <set>

#include "entity.h"

class EntityPool {
private:
    std::set<std::shared_ptr<Entity>> entitiesForDeletion;
    std::set<std::shared_ptr<Entity>> entities;

    void updateEntity(std::shared_ptr<Entity> entity, Uint32 timeSinceLastFrame, bool& quit);

public:
    EntityPool();

    void updateEntities(Uint32 timeSinceLastFrame, bool& quit);
    void drawEntities(std::shared_ptr<SDL_Renderer> renderer);

    std::shared_ptr<Entity> addEntity(std::shared_ptr<Entity> entity);
    std::set<std::shared_ptr<Entity>> getEntities(void);
    std::set<std::shared_ptr<Entity>> getEntitiesOnTile(const int& x, const int& y);
};