#include "application.h"

Application::Application() {
    grid = std::make_shared<Grid>(20, 20);
    window = std::make_shared<Window>(1024, 768, grid);
    turnController = std::make_shared<TurnController>(grid);
    areaOfEffectPool = std::make_shared<AreaOfEffectPool>(turnController, grid);
    projectilePool = std::make_shared<ProjectilePool>(areaOfEffectPool);
    weaponController = std::make_shared<WeaponController>(grid, projectilePool);
    entityPool = std::make_shared<EntityPool>(turnController, weaponController);

    context = std::make_shared<ApplicationContext>(
        window, 
        entityPool,
        weaponController,
        projectilePool, 
        areaOfEffectPool, 
        turnController
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