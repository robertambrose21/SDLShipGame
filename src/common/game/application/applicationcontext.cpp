#include "applicationcontext.h"

ApplicationContext::ApplicationContext(
    Grid* grid,
    EntityPool* entityPool,
    WeaponController* weaponController,
    ProjectilePool* projectilePool,
    AreaOfEffectPool* areaOfEffectPool,
    GameController* gameController,
    ItemController* itemController,
    EffectController* effectController,
    SpawnController* spawnController,
    VisiblityController* visiblityController
) :
    grid(grid),
    entityPool(entityPool),
    weaponController(weaponController),
    projectilePool(projectilePool),
    areaOfEffectPool(areaOfEffectPool),
    gameController(gameController),
    itemController(itemController),
    effectController(effectController),
    spawnController(spawnController),
    visiblityController(visiblityController)
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

GameController* ApplicationContext::getGameController(void) {
    return gameController;
}

ItemController* ApplicationContext::getItemController(void) {
    return itemController;
}

EffectController* ApplicationContext::getEffectController(void) {
    return effectController;
}

SpawnController* ApplicationContext::getSpawnController(void) {
    return spawnController;
}

VisiblityController* ApplicationContext::getVisibilityController(void) {
    return visiblityController;
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