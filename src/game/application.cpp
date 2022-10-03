#include "application.h"

Application::Application() {
    grid = std::make_shared<Grid>(20, 20);
    window = std::make_shared<Window>(1024, 768, grid);
    turnController = std::make_shared<TurnController>();
    entityPool = std::make_shared<EntityPool>();
    projectilePool = std::make_shared<ProjectilePool>();
    playerController = std::make_shared<PlayerController>(window->getGridRenderer(), entityPool);
    areaOfEffectPool = std::make_shared<AreaOfEffectPool>(turnController);

    context = std::make_shared<ApplicationContext>(window, entityPool, projectilePool, areaOfEffectPool, turnController);
}

void Application::initialise(void) {
    for(auto i = 0; i < grid->getWidth(); i++) {
        for(auto j = 0; j < grid->getHeight(); j++) {
            if(i == 10 && j != 12) {
                grid->setTile(i, j, { 2, false });
            }
            else {
                grid->setTile(i, j, { 1, true });
            }
        }
    }

    window->initialiseWindow();

    playerController->getEntity()->setTexture(window->getTextureLoader()->getTexture("../assets/player.png"));
    
    auto pistolTemp = std::make_shared<ProjectileWeapon>(
        playerController->getEntity(),
        window->getGridRenderer(), 
        "Pistol", 
        Weapon::Stats { 1, 100, 2 },
        Projectile::Blueprint(
            Projectile::Stats { 1, 50 },
            window->getTextureLoader()->getTexture("../assets/bullet.png"),
            [&](auto grid, auto entity, auto turnNumber) {
                areaOfEffectPool->add(std::make_shared<AreaOfEffect>(
                    window->getGridRenderer(), 
                    window->getTextureLoader()->getTexture("../assets/explosion.png"), 
                    turnNumber,
                    entity->getPosition(),
                    AreaOfEffect::Stats { 2.0f, 1 }
                ));
            }
        )
        
    );
    auto pistol = playerController->getEntity()->addWeapon(pistolTemp);
    playerController->getEntity()->setCurrentWeapon(pistol);
    entityPool->addEntity(playerController->getEntity());

    // TODO: Weapon blueprints
    auto enemy = entityPool->addEntity(
        std::make_shared<Enemy>(
            window->getGridRenderer(), 
            "Space Worm", 
            playerController->getEntity(), 
            Entity::Stats { 5, 2 }
    ));
    enemy->setPosition(glm::ivec2(0, grid->getHeight() - 1));
    enemy->setTexture(window->getTextureLoader()->getTexture("../assets/spaceworm.png"));
    auto teeth = std::make_shared<MeleeWeapon>(enemy, window->getGridRenderer(), "Teeth", (Weapon::Stats) { 1, 2, 1 });
    enemy->addWeapon(teeth);
    enemy->setCurrentWeapon(teeth);

    auto enemy2 = entityPool->addEntity(
        std::make_shared<Enemy>(
            window->getGridRenderer(), 
            "Space Worm", 
            playerController->getEntity(), 
            Entity::Stats { 5, 2 }
    ));
    enemy2->setPosition(glm::ivec2(5, grid->getHeight() - 3));
    enemy2->setTexture(window->getTextureLoader()->getTexture("../assets/spaceworm.png"));
    auto teeth2 = std::make_shared<MeleeWeapon>(enemy2, window->getGridRenderer(), "Teeth", (Weapon::Stats) { 1, 2, 1 });
    enemy->addWeapon(teeth2);
    enemy2->setCurrentWeapon(teeth2);

    auto enemy3 = entityPool->addEntity(
        std::make_shared<Enemy>(
            window->getGridRenderer(), 
            "Space Worm", 
            playerController->getEntity(), 
            Entity::Stats { 5, 2 }
    ));
    enemy3->setPosition(glm::ivec2(7, grid->getHeight() - 3));
    enemy3->setTexture(window->getTextureLoader()->getTexture("../assets/spaceworm.png"));
    auto teeth3 = std::make_shared<MeleeWeapon>(enemy3, window->getGridRenderer(), "Teeth", (Weapon::Stats) { 1, 2, 1 });
    enemy3->addWeapon(teeth3);
    enemy3->setCurrentWeapon(teeth3);

    turnController->addParticipant({ playerController->getEntity() }, true);
    turnController->addParticipant({ enemy, enemy2, enemy3 }, false);
    turnController->reset();

    window->addLoopDrawWorker([&](auto renderer, auto& quit) {
        entityPool->drawEntities(renderer);
        projectilePool->draw(renderer);
        areaOfEffectPool->draw(renderer);
    });
    window->addLoopLogicWorker([&](auto timeSinceLastFrame, auto& quit) {
        turnController->update(timeSinceLastFrame);
        entityPool->updateEntities(timeSinceLastFrame, quit);
        projectilePool->update(timeSinceLastFrame);
        areaOfEffectPool->update(timeSinceLastFrame);
    });
    window->addLoopEventWorker([&](auto e, auto& quit) {
        playerController->handleKeyPress(e);
        playerController->handleMouseEvent(e);
    });

    window->setGridTileTexture(1, "../assets/floor1.png");
    window->setGridTileTexture(2, "../assets/wall.png");
}

void Application::run(void) {
    window->loop();
}