#include "applicationcontext.h"

ApplicationContext::ApplicationContext(
    std::shared_ptr<Window> window,
    std::shared_ptr<EntityPool> entityPool,
    std::shared_ptr<ProjectilePool> projectilePool,
    std::shared_ptr<AreaOfEffectPool> areaOfEffectPool
) :
    window(window),
    entityPool(entityPool),
    projectilePool(projectilePool),
    areaOfEffectPool(areaOfEffectPool)
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