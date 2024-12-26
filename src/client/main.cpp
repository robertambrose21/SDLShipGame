#include "spdlog/spdlog.h" // Ensure this is loaded first to prevent macro clashes with yojimbo
#include "application/clientapplication.h"

int main() {
    yojimbo_log_level( YOJIMBO_LOG_LEVEL_INFO );

    #if !defined(NDEBUG)
        spdlog::set_level(spdlog::level::trace);
        spdlog::debug("Debug logging enabled");
    #else
        spdlog::set_level(spdlog::level::info);
    #endif

    ClientApplication application;
    application.initialise();
    application.run();

	return 0;
}