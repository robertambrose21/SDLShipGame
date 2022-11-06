#include "applicationcontext.h"

ApplicationContext::ApplicationContext(
    std::shared_ptr<Window> window,
    std::shared_ptr<EntityPool> entityPool,
    std::shared_ptr<ProjectilePool> projectilePool,
    std::shared_ptr<AreaOfEffectPool> areaOfEffectPool,
    std::shared_ptr<TurnController> turnController
) :
    window(window),
    entityPool(entityPool),
    projectilePool(projectilePool),
    areaOfEffectPool(areaOfEffectPool),
    turnController(turnController)
{ }

std::shared_ptr<Window> ApplicationContext::getWindow(void) {
    return window;
}

std::shared_ptr<EntityPool> ApplicationContext::getEntityPool(void) {
    return entityPool;
}

std::shared_ptr<ProjectilePool> ApplicationContext::getProjectilePool(void) {
    return projectilePool;
}

std::shared_ptr<AreaOfEffectPool> ApplicationContext::getAreaOfEffectPool(void) {
    return areaOfEffectPool;
}

std::shared_ptr<TurnController> ApplicationContext::getTurnController(void) {
    return turnController;
}

std::shared_ptr<Grid> ApplicationContext::getGrid(void) {
    return getWindow()->getGridRenderer()->getGrid();
}

std::shared_ptr<GraphicsContext> ApplicationContext::getGraphicsContext(void) {
    return getWindow()->getGraphicsContext();
}

// TODO: Batch updates if we have over max limit of GSU entities/other objects
GameStateUpdate ApplicationContext::getCurrentGameState(void) {
    auto entities = getEntityPool()->getEntities();

    GameStateUpdate update;
    update.numEntities = entities.size();

    int index = 0;
    for(auto [entityId, entity] : entities) {
        EntityStateUpdate entityStateUpdate;
        entityStateUpdate.id = entityId;
        entityStateUpdate.currentHP = entity->getCurrentHP();
        entityStateUpdate.x = entity->getPosition().x;
        entityStateUpdate.y = entity->getPosition().y;

        update.entities[index++] = entityStateUpdate;
    }

    return update;
}