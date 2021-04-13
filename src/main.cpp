#include "graphics/window.h"

int main() {
	Window test(1024, 768);
    test.initialiseWindow();
    test.loop();

	return 0;
}