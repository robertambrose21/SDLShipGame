#include "game/application/client/clientapplication.h"

int main() {
    yojimbo_log_level( YOJIMBO_LOG_LEVEL_INFO );

    ClientApplication application;
    application.initialise();
    application.run();

	return 0;
}