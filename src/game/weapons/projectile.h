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
        std::string name;
        uint8_t textureId;
        std::function<void(std::shared_ptr<Grid>, std::shared_ptr<Entity>, int)> onHitCallback;

        _blueprint(
            Stats stats,
            std::string name,
            uint8_t textureId,
            std::function<void(std::shared_ptr<Grid>, std::shared_ptr<Entity>, int)> onHitCallback =
                [](std::shared_ptr<Grid>, std::shared_ptr<Entity>, int){ }
        ) :
            stats(stats),
            name(name),
            textureId(textureId),
            onHitCallback(onHitCallback)
        { }
    } Blueprint;

private:
    Stats stats;

    std::shared_ptr<Grid> grid;
    uint8_t textureId;

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
        uint8_t textureId,
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
            blueprint.textureId, 
            startPosition, 
            target, 
            blueprint.stats, 
            weaponBaseDamage, 
            blueprint.onHitCallback
        );
    }

    void draw(std::shared_ptr<GraphicsContext> graphicsContext);
    void update(const uint32_t& timeSinceLastFrame);

    bool hasReachedTarget(void) const;
};