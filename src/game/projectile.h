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

    typedef struct _blueprint {
        Stats stats;
        std::shared_ptr<Texture> texture;
        std::function<void(std::shared_ptr<Grid>, std::shared_ptr<Entity>)> onHitCallback;

        _blueprint(
            Stats stats, 
            std::shared_ptr<Texture> texture,
            std::function<void(std::shared_ptr<Grid>, std::shared_ptr<Entity>)> onHitCallback =
                [](std::shared_ptr<Grid>, std::shared_ptr<Entity>){ }
        ) :
            stats(stats),
            texture(texture),
            onHitCallback(onHitCallback)
        { }
    } Blueprint;

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

    float getStep(void) const;

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

    static std::shared_ptr<Projectile> create(
        Blueprint blueprint, 
        std::shared_ptr<GridRenderer> grid,
        glm::ivec2 startPosition,
        std::shared_ptr<Entity> target,
        int weaponBaseDamage
    ) {
        return std::make_shared<Projectile>(
            grid, 
            blueprint.texture, 
            startPosition, 
            target, 
            blueprint.stats, 
            weaponBaseDamage, 
            blueprint.onHitCallback
        );
    }

    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);

    bool hasReachedTarget(void) const;
};