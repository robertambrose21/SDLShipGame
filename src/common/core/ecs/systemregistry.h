#pragma once

#include <entt/entt.hpp>
#include <cstdint>
#include <string>
#include <map>

#include "spdlog/spdlog.h"

class System {
public:
    System() = delete;
    System(const std::string name);

    virtual void update(entt::registry& registry, int64_t timeSinceLastFrame, bool& quit) = 0;

    std::string getName(void) const;

private:
    std::string name;
};

class SystemRegistry {
public:
    SystemRegistry() = delete;
    SystemRegistry(entt::registry& registry);

    void addSystem(std::unique_ptr<System> system);
    void removeSystem(const std::string& name);

    void update(int64_t timeSinceLastFrame, bool& quit);

    const std::map<std::string, std::unique_ptr<System>>& getAllRegisteredSystems(void) const;
    System* getSystem(const std::string& name);

private:
    std::map<std::string, std::unique_ptr<System>> systems;
    entt::registry& registry;
};