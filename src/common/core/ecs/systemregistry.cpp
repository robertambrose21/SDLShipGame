#include "systemregistry.h"

System::System(const std::string name) :
    name(name)
{ }

std::string System::getName(void) const {
    return name;
}


SystemRegistry::SystemRegistry(entt::registry& registry) :
    registry(registry)
{ }

void SystemRegistry::addSystem(std::unique_ptr<System> system) {
    if(systems.contains(system->getName())) {
        spdlog::warn("Cannot add system '{}'. System already exists", system->getName());
        return;
    }

    systems[system->getName()] = std::move(system);
}

void SystemRegistry::removeSystem(const std::string& name) {
    systems.erase(name);
}

void SystemRegistry::update(int64_t timeSinceLastFrame, bool& quit) {
    for(const auto& [_, system] : systems) {
        system->update(registry, timeSinceLastFrame, quit);
    }
}

const std::map<std::string, std::unique_ptr<System>>& SystemRegistry::getAllRegisteredSystems(void) const {
    return systems;
}

System* SystemRegistry::getSystem(const std::string& name) {
    if(!systems.contains(name)) {
        spdlog::debug("Cannot get system '{}'. System does not exist", name);
        return nullptr;
    }

    return systems[name].get();
}
