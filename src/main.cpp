// #include "game/serverapplication.h"
#include "game/clientapplication.h"

int main() {
    yojimbo_log_level( YOJIMBO_LOG_LEVEL_INFO );

    ClientApplication application;
    application.initialise();
    application.run();

    // Application::instance().initialise();
    // Application::instance().run();

	return 0;
}