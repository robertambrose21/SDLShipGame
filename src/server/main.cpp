#include "application/serverapplication.h"

// #include "core/grid/generation/wfc2.h"

int main() {
    yojimbo_log_level( YOJIMBO_LOG_LEVEL_INFO );

    // std::cout << "Hello there" << std::endl;

    // WFC2().run();

    ServerApplication application;
    application.initialise();
    application.run();

	return 0;
}