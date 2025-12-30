#pragma once

#include "core/ecs/systemregistry.h"
#include "game/application/applicationcontext.h"

class ApplicationContext;

class LogicSystem : public System {
public:
    LogicSystem() = delete;
    LogicSystem(const std::string& name);

    virtual void update(
        entt::registry& registry, 
        ApplicationContext& context, 
        int64_t timeSinceLastFrame, 
        bool& quit
    ) = 0;
};

class LogicSystemRegistry : public SystemRegistry<LogicSystem> {
public:
    LogicSystemRegistry() = delete;
    LogicSystemRegistry(entt::registry& registry);

    void update(ApplicationContext& context, int64_t timeSinceLastFrame, bool& quit);
};