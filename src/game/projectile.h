#pragma once

#include <functional>

#include "../core/glmimport.h"
#include "../graphics/gridrenderer.h"
#include "entity.h"

class Entity;

class Projectile {
public:
    typedef struct _stats {
        float damageMultiplier;
        float speed;
    } Stats;

private:
    Stats stats;

    std::shared_ptr<GridRenderer> grid;
    std::shared_ptr<Texture> texture;

    glm::ivec2 position;
    glm::ivec2 startPosition;
    std::shared_ptr<Entity> target;

    float timeSinceLive;
    float distanceToTarget;

    int weaponBaseDamage;

    std::function<void(std::shared_ptr<Grid>, std::shared_ptr<Entity>)> onHitCallback;

public:
    Projectile(
        std::shared_ptr<GridRenderer> grid,
        std::shared_ptr<Texture> texture,
        glm::ivec2 startPosition,
        std::shared_ptr<Entity> target,
        Stats stats,
        int weaponBaseDamage,
        std::function<void(std::shared_ptr<Grid>, std::shared_ptr<Entity>)> onHitCallback = 
            [](std::shared_ptr<Grid>, std::shared_ptr<Entity>){ }
    );

    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);

    bool hasReachedTarget(void) const;
};