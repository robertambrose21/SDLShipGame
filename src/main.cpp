#include "game/application/serverapplication.h"
// #include "game/application/clientapplication.h"

int main() {
    yojimbo_log_level( YOJIMBO_LOG_LEVEL_INFO );

    ServerApplication application;
    application.initialise();
    application.run();

    // Application::instance().initialise();
    // Application::instance().run();

	return 0;
}