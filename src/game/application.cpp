#include "application.h"

Application::Application() {
    grid = std::make_shared<Grid>(20, 20);
    window = std::make_shared<Window>(1024, 768, grid);
    entityPool = std::make_shared<EntityPool>();
    projectilePool = std::make_shared<ProjectilePool>();
    playerController = std::make_shared<PlayerController>(window->getGridRenderer(), entityPool);
    areaOfEffectPool = std::make_shared<AreaOfEffectPool>(entityPool);

    context = std::make_shared<ApplicationContext>(window, entityPool, projectilePool, areaOfEffectPool);
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
            [&](auto grid, auto entity) {
                areaOfEffectPool->add(std::make_shared<AreaOfEffect>(
                    window->getGridRenderer(), 
                    window->getTextureLoader()->getTexture("../assets/explosion.png"), 
                    entity->getPosition(),
                    AreaOfEffect::Stats { 2.0f, 1 }
                ));
            }
        )
        
    );
    // pistolTemp->setProjectileTexture(window.getTextureLoader()->getTexture("../assets/bullet.png"));
    auto pistol = playerController->getEntity()->addWeapon(pistolTemp);
    playerController->getEntity()->setCurrentWeapon(pistol);

    entityPool->setPlayerEntity(playerController->getEntity());

    // TODO: Weapon blueprints
    auto enemy = entityPool->createEntity(
        std::make_shared<Enemy>(
            window->getGridRenderer(), 
            "Space Worm", 
            playerController->getEntity(), 
            Entity::Stats { 5, 2 }
    ));
    enemy->setPosition(glm::ivec2(0, grid->getHeight() - 1));
    enemy->setTexture(window->getTextureLoader()->getTexture("../assets/spaceworm.png"));
    auto teeth = std::make_shared<MeleeWeapon>(enemy, window->getGridRenderer(), "Teeth", (Weapon::Stats) { 1, 1, 1 });
    enemy->setCurrentWeapon(teeth);

    auto enemy2 = entityPool->createEntity(
        std::make_shared<Enemy>(
            window->getGridRenderer(), 
            "Space Worm", 
            playerController->getEntity(), 
            Entity::Stats { 5, 2 }
    ));
    enemy2->setPosition(glm::ivec2(5, grid->getHeight() - 3));
    enemy2->setTexture(window->getTextureLoader()->getTexture("../assets/spaceworm.png"));
    auto teeth2 = std::make_shared<MeleeWeapon>(enemy2, window->getGridRenderer(), "Teeth", (Weapon::Stats) { 1, 1, 1 });
    enemy2->setCurrentWeapon(teeth2);

    window->addLoopDrawWorker([&](auto renderer, auto& quit) {
        entityPool->drawEntities(renderer);
        projectilePool->draw(renderer);
        areaOfEffectPool->draw(renderer);
    });
    window->addLoopLogicWorker([&](auto timeSinceLastFrame, auto& quit) {
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