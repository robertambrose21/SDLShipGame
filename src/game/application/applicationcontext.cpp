#include "applicationcontext.h"

ApplicationContext::ApplicationContext(
    const std::shared_ptr<Window>& window,
    const std::shared_ptr<EntityPool>& entityPool,
    const std::shared_ptr<WeaponController>& weaponController,
    const std::shared_ptr<ProjectilePool>& projectilePool,
    const std::shared_ptr<AreaOfEffectPool>& areaOfEffectPool,
    const std::shared_ptr<TurnController>& turnController
) :
    window(window),
    entityPool(entityPool),
    weaponController(weaponController),
    projectilePool(projectilePool),
    areaOfEffectPool(areaOfEffectPool),
    turnController(turnController)
{
    game_assert(window != nullptr);
    game_assert(entityPool != nullptr);
    game_assert(weaponController != nullptr);
    game_assert(projectilePool != nullptr);
    game_assert(areaOfEffectPool != nullptr);
    game_assert(turnController != nullptr);
}

std::shared_ptr<Window> ApplicationContext::getWindow(void) {
    return window;
}

std::shared_ptr<EntityPool> ApplicationContext::getEntityPool(void) {
    return entityPool;
}

std::shared_ptr<WeaponController> ApplicationContext::getWeaponController(void) {
    return weaponController;
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

void ApplicationContext::setServerMessagesTransmitter(const std::shared_ptr<ServerMessagesTransmitter>& transmitter) {
    game_assert(transmitter != nullptr);
    this->transmitter = transmitter;
}

std::shared_ptr<ServerMessagesTransmitter> ApplicationContext::getServerMessagesTransmitter(void) {
    game_assert(transmitter != nullptr);
    return transmitter;
}

// TODO: Batch updates if we have over max limit of GSU entities/other objects
// TODO: Consider moving this elsewhere
GameStateUpdate ApplicationContext::getCurrentGameState(void) {
    return GameStateUpdate::serialize(getTurnController()->getCurrentParticipant(), getEntityPool()->getEntities());
}