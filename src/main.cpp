#include "game/application.h"

int main() {
    Application::instance().initialise();
    Application::instance().run();

	return 0;
}