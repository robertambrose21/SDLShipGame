#include "graphics/window.h"
#include "player/playercontroller.h"
#include "game/enemy.h"

int main() {
	Window window(1024, 768);
    window.initialiseWindow();

    auto player = std::make_shared<PlayerController>(window.getGrid(), 2);
    player->setTexture(window.getTextureLoader()->getTexture("../assets/player.png"));

    auto enemy = std::make_shared<Enemy>(window.getGrid(), "Space Worm", player, 1);
    enemy->setPosition(glm::ivec2(0, window.getGrid()->getHeight() - 1));
    enemy->setTexture(window.getTextureLoader()->getTexture("../assets/spaceworm.png"));

    window.addLoopDrawWorker([&](auto renderer, bool& quit) {
        player->draw(renderer);
        enemy->draw(renderer);
    });
    window.addLoopLogicWorker([&](const Uint32& timeSinceLastFrame, bool& quit) {
        player->update(timeSinceLastFrame, quit);
        enemy->update(timeSinceLastFrame, quit);
    });
    window.addLoopEventWorker([&](SDL_Event e, bool& quit) {
        player->handleKeyPress(e);
    });

    for(auto i = 0; i < 20; i++) {
        for(auto j = 0; j < 20; j++) {
            window.setGridTileTexture(i, j, "../assets/floor1.png");
        }
    }

    window.loop();

	return 0;
}