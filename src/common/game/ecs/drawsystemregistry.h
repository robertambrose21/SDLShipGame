#pragma once

#include "core/ecs/systemregistry.h"
#include "graphics/graphicscontext.h"

class DrawSystem : public System {
public:
    DrawSystem() = delete;
    DrawSystem(const std::string& name);

    virtual void draw(entt::registry& registry, GraphicsContext& graphicsContext) = 0;
};

class DrawSystemRegistry : public SystemRegistry<DrawSystem> {
public:
    DrawSystemRegistry() = delete;
    DrawSystemRegistry(entt::registry& registry);

    void draw(GraphicsContext& graphicsContext);
};