#include "graphics/window.h"

int main() {
	Window window(1024, 768);
    window.initialiseWindow();

    for(auto i = 0; i < 20; i++) {
        for(auto j = 0; j < 20; j++) {
            window.setGridTileTexture(i, j, "../assets/floor1.png");
        }
    }

    window.loop();

	return 0;
}