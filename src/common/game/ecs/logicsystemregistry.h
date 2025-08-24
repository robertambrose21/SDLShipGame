#pragma once

#include "core/ecs/systemregistry.h"

class LogicSystem : public System {
public:
    virtual void update(entt::registry& registry, int64_t timeSinceLastFrame, bool& quit) = 0;
};

class LogicSystemRegistry : public SystemRegistry<LogicSystem> {
public:
    LogicSystemRegistry() = delete;
    LogicSystemRegistry(entt::registry& registry);

    void update(int64_t timeSinceLastFrame, bool& quit);
};