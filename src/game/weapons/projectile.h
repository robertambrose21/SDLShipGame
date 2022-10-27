#pragma once

#include <functional>

#include "core/glmimport.h"
#include "graphics/gridrenderer.h"
#include "game/entities/entity.h"

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
        std::function<void(std::shared_ptr<Grid>, std::shared_ptr<Entity>, int)> onHitCallback;

        _blueprint(
            Stats stats, 
            std::shared_ptr<Texture> texture,
            std::function<void(std::shared_ptr<Grid>, std::shared_ptr<Entity>, int)> onHitCallback =
                [](std::shared_ptr<Grid>, std::shared_ptr<Entity>, int){ }
        ) :
            stats(stats),
            texture(texture),
            onHitCallback(onHitCallback)
        { }
    } Blueprint;

private:
    Stats stats;

    std::shared_ptr<Grid> grid;
    std::shared_ptr<Texture> texture;

    glm::ivec2 position;
    glm::ivec2 startPosition;
    std::shared_ptr<Entity> target;

    float timeSinceLive;
    float distanceToTarget;

    int weaponBaseDamage;

    std::function<void(std::shared_ptr<Grid>, std::shared_ptr<Entity>, int)> onHitCallback;

    float getStep(void) const;

public:
    Projectile(
        std::shared_ptr<Grid> grid,
        std::shared_ptr<Texture> texture,
        glm::ivec2 startPosition,
        std::shared_ptr<Entity> target,
        Stats stats,
        int weaponBaseDamage,
        std::function<void(std::shared_ptr<Grid>, std::shared_ptr<Entity>, int)> onHitCallback = 
            [](std::shared_ptr<Grid>, std::shared_ptr<Entity>, int){ }
    );

    static std::shared_ptr<Projectile> create(
        std::shared_ptr<Grid> grid,
        Blueprint blueprint, 
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

    void draw(std::shared_ptr<SDL_Renderer> renderer, std::shared_ptr<GridRenderer> gridRenderer);
    void update(const Uint32& timeSinceLastFrame);

    bool hasReachedTarget(void) const;
};