#pragma once

#include "game/ecs/components/drawable.h"
#include "game/ecs/drawsystemregistry.h"

class ActorDrawSystem : public DrawSystem {
public:
    ActorDrawSystem() = delete;
    ActorDrawSystem(const std::string& name);

    void update(entt::registry& registry, GraphicsContext& graphicsContext, int64_t timeSinceLastFrame);
};