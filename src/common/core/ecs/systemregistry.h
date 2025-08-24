#pragma once

#include <entt/entt.hpp>
#include <cstdint>
#include <string>
#include <map>
#include <concepts>

#include "spdlog/spdlog.h"

class System {
public:
    System() = delete;
    System(const std::string name) :
        name(name)
    { }
    virtual ~System() = default;

    std::string getName(void) const {
        return name;
    }

private:
    std::string name;
};

template <typename T>
requires std::derived_from<T, System>
class SystemRegistry {
public:
    SystemRegistry() = delete;
    SystemRegistry(entt::registry& registry) : 
        registry(registry)
    { }

    void addSystem(std::unique_ptr<T> system) {
        if(systems.contains(system->getName())) {
            spdlog::warn("Cannot add system '{}'. System already exists", system->getName());
            return;
        }

        systems[system->getName()] = std::move(system);
    }

    void removeSystem(const std::string& name) {
        systems.erase(name);
    }

    const std::map<std::string, std::unique_ptr<T>>& getAllRegisteredSystems(void) const {
        return systems;
    }

    T* getSystem(const std::string& name) {
        if(!systems.contains(name)) {
            spdlog::debug("Cannot get system '{}'. System does not exist", name);
            return nullptr;
        }

        return systems[name].get();
    }

    entt::registry& getRegistry(void) {
        return registry;
    }

private:
    std::map<std::string, std::unique_ptr<T>> systems;
    entt::registry& registry;
};