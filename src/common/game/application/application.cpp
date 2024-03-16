#include "application.h"

Application::Application(
    std::unique_ptr<Grid> grid,
    std::unique_ptr<EntityPool> entityPool,
    std::unique_ptr<WeaponController> weaponController,
    std::unique_ptr<ProjectilePool> projectilePool,
    std::unique_ptr<AreaOfEffectPool> areaOfEffectPool,
    std::unique_ptr<TurnController> turnController,
    std::unique_ptr<ItemController> itemController,
    std::unique_ptr<EffectController> effectController
) :
    grid(std::move(grid)),
    entityPool(std::move(entityPool)),
    weaponController(std::move(weaponController)),
    projectilePool(std::move(projectilePool)),
    areaOfEffectPool(std::move(areaOfEffectPool)),
    turnController(std::move(turnController)),
    itemController(std::move(itemController)),
    effectController(std::move(effectController))
{
    context = std::make_unique<ApplicationContext>(
        this->grid.get(),
        this->entityPool.get(),
        this->weaponController.get(),
        this->projectilePool.get(), 
        this->areaOfEffectPool.get(), 
        this->turnController.get(),
        this->itemController.get(),
        this->effectController.get()
    );
}

ApplicationContext& Application::getContext(void) {
    return *context;
}

void Application::addLogicWorker(std::function<void(ApplicationContext&, int64_t, bool&)> func) {
    logicWorkers.push_back(func);
}

void Application::run(void) {
    loop();
}

void Application::loop(void) {
    int64_t currentTime = getCurrentTimeInMilliseconds();
    int64_t timeSinceLastFrame = 0;
    bool quit = false;

    while(!quit) {
        timeSinceLastFrame = getCurrentTimeInMilliseconds() - currentTime;
        currentTime = getCurrentTimeInMilliseconds();

        for(auto const& worker : logicWorkers) {
            worker(getContext(), timeSinceLastFrame, quit);
        }
    }
}