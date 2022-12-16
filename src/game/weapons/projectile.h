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
        std::function<void(const std::shared_ptr<Grid>&, const std::shared_ptr<Entity>&, const int&)> onHitCallback;

        _blueprint(
            const Stats& stats,
            const std::string& name,
            const uint8_t& textureId,
            std::function<void(const std::shared_ptr<Grid>&, const std::shared_ptr<Entity>&, const int&)> onHitCallback =
                [](const std::shared_ptr<Grid>&, const std::shared_ptr<Entity>&, const int&){ }
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

    std::function<void(const std::shared_ptr<Grid>&, const std::shared_ptr<Entity>&, const int&)> onHitCallback;

    float getStep(void) const;

public:
    Projectile(
        const std::shared_ptr<Grid>& grid,
        const uint8_t& textureId,
        const glm::ivec2& startPosition,
        const std::shared_ptr<Entity>& target,
        const Stats& stats,
        const int& weaponBaseDamage,
        std::function<void(const std::shared_ptr<Grid>&, const std::shared_ptr<Entity>&, const int&)> onHitCallback = 
            [](const std::shared_ptr<Grid>&, const std::shared_ptr<Entity>&, const int&){ }
    );

    static std::shared_ptr<Projectile> create(
        const std::shared_ptr<Grid>& grid,
        const Blueprint& blueprint, 
        const glm::ivec2& startPosition,
        const std::shared_ptr<Entity>& target,
        const int& weaponBaseDamage
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

    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);
    void update(const uint32_t& timeSinceLastFrame);

    bool hasReachedTarget(void) const;
};