#include "logicsystemregistry.h"

LogicSystem::LogicSystem(const std::string& name) :
    System(name)
{ }


LogicSystemRegistry::LogicSystemRegistry(entt::registry& registry) :
    SystemRegistry(registry)
{ }

void LogicSystemRegistry::update(ApplicationContext& context, int64_t timeSinceLastFrame, bool& quit) {
    for(const auto& [name, system] : getAllRegisteredSystems()) {
        system->update(getRegistry(), context, timeSinceLastFrame, quit);
    }
}