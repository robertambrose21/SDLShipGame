#include "logicsystemregistry.h"

LogicSystemRegistry::LogicSystemRegistry(entt::registry& registry) :
    SystemRegistry(registry)
{ }

void LogicSystemRegistry::update(int64_t timeSinceLastFrame, bool& quit) {
    for(const auto& [name, system] : getAllRegisteredSystems()) {
        system->update(getRegistry(), timeSinceLastFrame, quit);
    }
}