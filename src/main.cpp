#include "graphics/window.h"
#include "player/playercontroller.h"

int main() {
	Window window(1024, 768);
    window.initialiseWindow();

    PlayerController player(window.getGrid(), 2);
    player.setTexture(window.getTextureLoader()->getTexture("../assets/player.png"));

    window.addLoopDrawWorker([&](auto renderer, bool& quit) {
        player.draw(renderer);
    });
    window.addLoopLogicWorker([&](const Uint32& timeSinceLastFrame, bool& quit) {
        player.update(timeSinceLastFrame, quit);
    });
    window.addLoopEventWorker([&](SDL_Event e, bool& quit) {
        player.handleKeyPress(e);
    });

    for(auto i = 0; i < 20; i++) {
        for(auto j = 0; j < 20; j++) {
            window.setGridTileTexture(i, j, "../assets/floor1.png");
        }
    }

    window.loop();

	return 0;
}