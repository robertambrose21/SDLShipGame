#include "graphics/window.h"
#include "player/playercontroller.h"
#include "game/enemy.h"

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

    auto player = std::make_shared<PlayerController>(window.getGridRenderer());
    player->getEntity()->setTexture(window.getTextureLoader()->getTexture("../assets/player.png"));

    // TODO: Figure out why we can't just inline this struct
    Entity::Stats s;
    s.movesPerTurn = 1;
    s.hp = 1;

    auto enemy = std::make_shared<Enemy>(window.getGridRenderer(), "Space Worm", player, s);
    enemy->setPosition(glm::ivec2(0, grid->getHeight() - 1));
    enemy->setTexture(window.getTextureLoader()->getTexture("../assets/spaceworm.png"));

    window.addLoopDrawWorker([&](auto renderer, bool& quit) {
        player->getEntity()->draw(renderer);
        enemy->draw(renderer);
    });
    window.addLoopLogicWorker([&](const Uint32& timeSinceLastFrame, bool& quit) {
        player->getEntity()->update(timeSinceLastFrame, quit);
        enemy->update(timeSinceLastFrame, quit);
    });
    window.addLoopEventWorker([&](SDL_Event e, bool& quit) {
        player->handleKeyPress(e);
        player->handleMouseEvent(e);
    });

    window.setGridTileTexture(1, "../assets/floor1.png");
    window.setGridTileTexture(2, "../assets/wall.png");

    window.loop();

	return 0;
}