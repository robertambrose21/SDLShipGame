#include "applicationcontext.h"

ApplicationContext::ApplicationContext(
    Window& window,
    EntityPool& entityPool,
    WeaponController& weaponController,
    ProjectilePool& projectilePool,
    AreaOfEffectPool& areaOfEffectPool,
    TurnController& turnController
) :
    window(window),
    entityPool(entityPool),
    weaponController(weaponController),
    projectilePool(projectilePool),
    areaOfEffectPool(areaOfEffectPool),
    turnController(turnController)
{ }

Window& ApplicationContext::getWindow(void) {
    return window;
}

EntityPool& ApplicationContext::getEntityPool(void) {
    return entityPool;
}

WeaponController& ApplicationContext::getWeaponController(void) {
    return weaponController;
}

ProjectilePool& ApplicationContext::getProjectilePool(void) {
    return projectilePool;
}

AreaOfEffectPool& ApplicationContext::getAreaOfEffectPool(void) {
    return areaOfEffectPool;
}

TurnController& ApplicationContext::getTurnController(void) {
    return turnController;
}

// TODO: Grid probably shouldn't come from the renderer?
Grid& ApplicationContext::getGrid(void) {
    return getWindow().getGridRenderer().getGrid();
}

GraphicsContext& ApplicationContext::getGraphicsContext(void) {
    return getWindow().getGraphicsContext();
}

void ApplicationContext::setServerMessagesTransmitter(ServerMessagesTransmitter* transmitter) {
    game_assert(transmitter != NULL);
    this->transmitter = transmitter;
}

ServerMessagesTransmitter* ApplicationContext::getServerMessagesTransmitter(void) {
    return transmitter;
}

// TODO: Batch updates if we have over max limit of GSU entities/other objects
// TODO: Consider moving this elsewhere
GameStateUpdate ApplicationContext::getCurrentGameState(void) {
    return GameStateUpdate::serialize(getTurnController().getCurrentParticipant(), getEntityPool().getEntities());
}