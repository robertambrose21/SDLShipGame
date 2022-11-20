#include "application.h"

Application::Application() {
    grid = std::make_shared<Grid>(20, 20);
    window = std::make_shared<Window>(1024, 768, grid);
    turnController = std::make_shared<TurnController>();
    entityPool = std::make_shared<EntityPool>();
    projectilePool = std::make_shared<ProjectilePool>();
    areaOfEffectPool = std::make_shared<AreaOfEffectPool>(turnController);

    context = std::make_shared<ApplicationContext>(window, entityPool, projectilePool, areaOfEffectPool, turnController);
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
    auto enemy = entityPool->addEntity(
        std::make_shared<Entity>(
            "Space Worm", 
            Entity::Stats { 5, 2 }
    ));
    enemy->setPosition(glm::ivec2(0, grid->getHeight() - 1));
    enemy->setTextureId(7);
    enemy->setSelectedTextureId(6);
    auto teeth = std::make_shared<MeleeWeapon>(enemy, window->getGridRenderer()->getGrid(), "Teeth", (Weapon::Stats) { 1, 2, 1 });
    enemy->addWeapon(teeth);
    enemy->setCurrentWeapon(teeth);
    enemy->setBehaviourStrategy(std::make_shared<ChaseAndAttackStrategy>(enemy));

    auto enemy2 = entityPool->addEntity(
        std::make_shared<Entity>(
            "Space Worm", 
            Entity::Stats { 5, 2 }
    ));
    enemy2->setPosition(glm::ivec2(5, grid->getHeight() - 3));
    enemy2->setTextureId(7);
    enemy2->setSelectedTextureId(6);
    auto teeth2 = std::make_shared<MeleeWeapon>(enemy2, window->getGridRenderer()->getGrid(), "Teeth", (Weapon::Stats) { 1, 2, 1 });
    enemy2->addWeapon(teeth2);
    enemy2->setCurrentWeapon(teeth2);
    enemy2->setBehaviourStrategy(std::make_shared<ChaseAndAttackStrategy>(enemy2));

    auto enemy3 = entityPool->addEntity(
        std::make_shared<Entity>(
            "Space Worm", 
            Entity::Stats { 5, 2 }
    ));
    enemy3->setPosition(glm::ivec2(17, grid->getHeight() - 3));
    enemy3->setTextureId(7);
    enemy3->setSelectedTextureId(6);
    auto teeth3 = std::make_shared<MeleeWeapon>(enemy3, window->getGridRenderer()->getGrid(), "Teeth", (Weapon::Stats) { 1, 2, 1 });
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
    auto player = entityPool->addEntity(std::make_shared<Entity>("Player", Entity::Stats { 3, 10 }));
    player->setTextureId(1);
    player->setSelectedTextureId(6);
    player->setPosition(position);
    
    auto pistolTemp = std::make_shared<ProjectileWeapon>(
        player,
        window->getGridRenderer()->getGrid(), 
        "Pistol", 
        Weapon::Stats { 1, 100, 2 },
        Projectile::Blueprint(
            Projectile::Stats { 1, 50 },
            2,
            [&](auto grid, auto entity, auto turnNumber) {
                areaOfEffectPool->add(std::make_shared<AreaOfEffect>(
                    window->getGridRenderer()->getGrid(), 
                    3,
                    turnNumber,
                    entity->getPosition(),
                    AreaOfEffect::Stats { 2.0f, 1 }
                ));
            }
        )
        
    );
    auto pistol = player->addWeapon(pistolTemp);
    player->setCurrentWeapon(pistol);
    return player;
}

void Application::run(void) {
    window->loop();
}