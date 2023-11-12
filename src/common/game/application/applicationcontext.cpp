#include "applicationcontext.h"

ApplicationContext::ApplicationContext(
    Grid* grid,
    EntityPool* entityPool,
    WeaponController* weaponController,
    ProjectilePool* projectilePool,
    AreaOfEffectPool* areaOfEffectPool,
    TurnController* turnController,
    ItemController* itemController
) :
    grid(grid),
    entityPool(entityPool),
    weaponController(weaponController),
    projectilePool(projectilePool),
    areaOfEffectPool(areaOfEffectPool),
    turnController(turnController),
    itemController(itemController)
{ }

EntityPool* ApplicationContext::getEntityPool(void) {
    return entityPool;
}

WeaponController* ApplicationContext::getWeaponController(void) {
    return weaponController;
}

ProjectilePool* ApplicationContext::getProjectilePool(void) {
    return projectilePool;
}

AreaOfEffectPool* ApplicationContext::getAreaOfEffectPool(void) {
    return areaOfEffectPool;
}

TurnController* ApplicationContext::getTurnController(void) {
    return turnController;
}

ItemController* ApplicationContext::getItemController(void) {
    return itemController;
}

Grid* ApplicationContext::getGrid(void) {
    return grid;
}

void ApplicationContext::setServerMessagesTransmitter(ServerMessagesTransmitter* transmitter) {
    game_assert(transmitter != NULL);
    this->transmitter = transmitter;
}

ServerMessagesTransmitter* ApplicationContext::getServerMessagesTransmitter(void) {
    game_assert(transmitter != NULL);
    return transmitter;
}