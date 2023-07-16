#include "application.h"

Application::Application() {
    grid = std::make_unique<Grid>(100, 100);
    window = std::make_unique<Window>(1920, 1080, *grid);
    turnController = std::make_unique<TurnController>(*grid);
    areaOfEffectPool = std::make_unique<AreaOfEffectPool>(*turnController, *grid);
    projectilePool = std::make_unique<ProjectilePool>(*areaOfEffectPool);
    weaponController = std::make_unique<WeaponController>(*grid, *projectilePool);
    entityPool = std::make_unique<EntityPool>(*turnController, *weaponController);

    context = std::make_unique<ApplicationContext>(
        *window, 
        *entityPool,
        *weaponController,
        *projectilePool, 
        *areaOfEffectPool, 
        *turnController
    );
}

Application::~Application()
{ }

void Application::initialise(const Window::Headless& headless) {
    window->initialiseWindow(headless);

    window->setGridTileTexture(1, 4);
    window->setGridTileTexture(2, 5);
}

void Application::run(void) {
    window->loop();
}