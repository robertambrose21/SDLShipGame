#include "graphics/window.h"
#include "player/playercontroller.h"
#include "game/enemy.h"
#include "game/projectileweapon.h"
#include "game/meleeweapon.h"

int main() {
    auto grid = std::make_shared<Grid>(20, 20);

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

	Window window(1024, 768, grid);
    window.initialiseWindow();

    auto entityPool = std::make_shared<EntityPool>();
    auto playerController = std::make_shared<PlayerController>(window.getGridRenderer(), entityPool);
    playerController->getEntity()->setTexture(window.getTextureLoader()->getTexture("../assets/player.png"));
    
    auto pistolTemp = std::make_shared<ProjectileWeapon>(window.getGridRenderer(), "Pistol", Weapon::Stats { 1, 100, 8 });
    pistolTemp->setProjectileTexture(window.getTextureLoader()->getTexture("../assets/bullet.png"));
    auto pistol = playerController->getEntity()->addWeapon(pistolTemp);
    playerController->setCurrentWeapon(pistol);

    entityPool->setPlayerEntity(playerController->getEntity());

    auto teeth = std::make_shared<MeleeWeapon>(window.getGridRenderer(), "Teeth", (Weapon::Stats) { 1, 1, -1 });

    auto enemy = entityPool->createEntity(
        std::make_shared<Enemy>(
            window.getGridRenderer(), 
            "Space Worm", 
            playerController->getEntity(), 
            Entity::Stats { 5, 1 },
            std::vector<std::shared_ptr<Weapon>> { teeth })
    );
    enemy->setPosition(glm::ivec2(0, grid->getHeight() - 1));
    enemy->setTexture(window.getTextureLoader()->getTexture("../assets/spaceworm.png"));

    auto enemy2 = entityPool->createEntity(
        std::make_shared<Enemy>(
            window.getGridRenderer(), 
            "Space Worm", 
            playerController->getEntity(), 
            Entity::Stats { 5, 1 },
            std::vector<std::shared_ptr<Weapon>> { teeth })
    );
    enemy2->setPosition(glm::ivec2(5, grid->getHeight() - 3));
    enemy2->setTexture(window.getTextureLoader()->getTexture("../assets/spaceworm.png"));

    window.addLoopDrawWorker([&](auto renderer, auto& quit) {
        entityPool->drawEntities(renderer);
    });
    window.addLoopLogicWorker([&](auto timeSinceLastFrame, auto& quit) {
        entityPool->updateEntities(timeSinceLastFrame, quit);
    });
    window.addLoopEventWorker([&](auto e, auto& quit) {
        playerController->handleKeyPress(e);
        playerController->handleMouseEvent(e);
    });

    window.setGridTileTexture(1, "../assets/floor1.png");
    window.setGridTileTexture(2, "../assets/wall.png");

    window.loop();

	return 0;
}