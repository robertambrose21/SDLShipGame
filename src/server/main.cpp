#include "application/serverapplication.h"

int main() {
    yojimbo_log_level( YOJIMBO_LOG_LEVEL_INFO );

    // std::vector<int> weights = { 8, 1 };

    // for(int i = 0; i < 100; i ++) {
    //     static std::default_random_engine generator;
    //     std::discrete_distribution<int> dist(weights.begin(), weights.end());
    //     std::cout << dist(generator) << std::endl;
    // }


    ServerApplication application;
    application.initialise();
    application.run();

	return 0;
}