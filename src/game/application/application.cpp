#include "application.h"

Application::Application() {
    grid = std::make_shared<Grid>(20, 20);
    window = std::make_shared<Window>(1024, 768, grid);
    turnController = std::make_shared<TurnController>();
    entityPool = std::make_shared<EntityPool>();
    areaOfEffectPool = std::make_shared<AreaOfEffectPool>(turnController, grid);
    projectilePool = std::make_shared<ProjectilePool>(areaOfEffectPool);
    weaponController = std::make_shared<WeaponController>(grid, projectilePool);

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

void Application::initialise(Window::Headless headless) {
    // for(auto i = 0; i < grid->getWidth(); i++) {
    //     for(auto j = 0; j < grid->getHeight(); j++) {
    //         if(i == 10 && j != 12) {
    //             grid->setTile(i, j, { 2, false });
    //         }
    //         else {
    //             grid->setTile(i, j, { 1, true });
    //         }
    //     }
    // }

    window->initialiseWindow(headless);
    
    auto player = addPlayer(glm::ivec2(0, 0));
    auto player2 = addPlayer(glm::ivec2(2, 1));

    // TODO: Weapon blueprints
    auto enemy = entityPool->addEntity("Space Worm");
    enemy->setPosition(glm::ivec2(0, grid->getHeight() - 1));
    auto teeth = weaponController->createWeapon("Space Worm Teeth", enemy);
    enemy->addWeapon(teeth);
    enemy->setCurrentWeapon(teeth);
    enemy->setBehaviourStrategy(std::make_shared<ChaseAndAttackStrategy>(enemy));

    auto enemy2 = entityPool->addEntity("Space Worm");
    enemy2->setPosition(glm::ivec2(5, grid->getHeight() - 3));
    auto teeth2 = weaponController->createWeapon("Space Worm Teeth", enemy2);
    enemy2->addWeapon(teeth2);
    enemy2->setCurrentWeapon(teeth2);
    enemy2->setBehaviourStrategy(std::make_shared<ChaseAndAttackStrategy>(enemy2));

    auto enemy3 = entityPool->addEntity("Space Worm");
    enemy3->setPosition(glm::ivec2(17, grid->getHeight() - 3));
    auto teeth3 = weaponController->createWeapon("Space Worm Teeth", enemy3);
    enemy3->addWeapon(teeth3);
    enemy3->setCurrentWeapon(teeth3);
    enemy3->setBehaviourStrategy(std::make_shared<ChaseAndAttackStrategy>(enemy3));

    turnController->addParticipant({ player, player2 }, true);
    turnController->addParticipant({ enemy, enemy2 }, false);
    turnController->addParticipant({ enemy3 }, false);
    turnController->reset();

    window->addLoopDrawWorker([&](auto graphicsContext, auto& quit) {
        entityPool->drawEntities(graphicsContext);
        projectilePool->draw(graphicsContext);
        areaOfEffectPool->draw(graphicsContext);
    });
    window->addLoopLogicWorker([&](auto timeSinceLastFrame, auto& quit) {
        turnController->update(timeSinceLastFrame);
        entityPool->updateEntities(timeSinceLastFrame, quit);
        projectilePool->update(timeSinceLastFrame);
        areaOfEffectPool->update(timeSinceLastFrame);
    });

    window->setGridTileTexture(1, 4);
    window->setGridTileTexture(2, 5);
}

std::shared_ptr<Entity> Application::addPlayer(glm::ivec2 position) {
    auto player = entityPool->addEntity("Player");
    player->setPosition(position);
    auto grenadeLauncher = player->addWeapon(weaponController->createWeapon("Grenade Launcher", player));
    player->setCurrentWeapon(grenadeLauncher);
    return player;
}

void Application::run(void) {
    window->loop();
}