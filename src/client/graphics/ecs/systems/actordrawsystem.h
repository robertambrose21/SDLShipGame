#pragma once

#include "game/ecs/components/common.h"
#include "game/ecs/drawsystemregistry.h"

class ActorDrawSystem : public DrawSystem {
public:
    ActorDrawSystem() = delete;
    ActorDrawSystem(const std::string& name);

    void draw(entt::registry& registry, GraphicsContext& graphicsContext);

private:
    void drawHealthBar(
        GraphicsContext& graphicsContext, 
        const glm::ivec2& position,
        int totalHP,
        int currentHP
    );
};