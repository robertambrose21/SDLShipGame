#include "application.h"

Application::Application() {
    grid = std::make_unique<Grid>(250, 250);
    turnController = std::make_unique<TurnController>(*grid);
    areaOfEffectPool = std::make_unique<AreaOfEffectPool>(*turnController, *grid);
    projectilePool = std::make_unique<ProjectilePool>(*areaOfEffectPool);
    weaponController = std::make_unique<WeaponController>(*grid, *projectilePool);
    entityPool = std::make_unique<EntityPool>(*turnController, *weaponController);

    context = std::make_unique<ApplicationContext>(
        *grid,
        *entityPool,
        *weaponController,
        *projectilePool, 
        *areaOfEffectPool, 
        *turnController
    );
}

Application::~Application()
{ }

void Application::initialise(void)
{ }

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
            worker(timeSinceLastFrame, quit);
        }
    }
}