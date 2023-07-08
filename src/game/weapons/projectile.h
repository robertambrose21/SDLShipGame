#pragma once

#include <functional>
#include <algorithm>

#include "core/glmimport.h"
#include "graphics/gridrenderer.h"
#include "game/entities/entity.h"
#include "game/effects/effect.h"

class Entity;

class Projectile {
public:
    typedef struct _stats {
        float damageMultiplier;
        float speed;
        std::vector<Effect> effects;
    } Stats;

    typedef struct _blueprint {
        Stats stats;
        std::string name;
        uint32_t textureId;
        std::function<void(const std::shared_ptr<Grid>&, int, const glm::ivec2&, int)> onHitCallback;

        _blueprint(
            const Stats& stats,
            const std::string& name,
            uint32_t textureId,
            std::function<void(const std::shared_ptr<Grid>&, int, const glm::ivec2&, int)> onHitCallback =
                [](const std::shared_ptr<Grid>&, int, const glm::ivec2&, int){ }
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
    uint32_t textureId;

    glm::ivec2 position;
    glm::ivec2 startPosition;
    glm::ivec2 target;

    int ownerId;

    float timeSinceLive;
    float distanceToTarget;
    float step;

    int weaponBaseDamage;

    // TODO: position not entity for target
    std::function<void(const std::shared_ptr<Grid>&, int, const glm::ivec2&, int)> onHitCallback;

    float calculateStep(void) const;
    void doHit(const glm::ivec2& position);

public:
    Projectile(
        const std::shared_ptr<Grid>& grid,
        uint32_t textureId,
        int ownerId,
        const glm::ivec2& startPosition,
        const glm::ivec2& target,
        const Stats& stats,
        int weaponBaseDamage,
        std::function<void(const std::shared_ptr<Grid>&, int, const glm::ivec2&, int)> onHitCallback = 
            [](const std::shared_ptr<Grid>&, int, const glm::ivec2&, int){ }
    );

    static std::shared_ptr<Projectile> create(
        const std::shared_ptr<Grid>& grid,
        int ownerId,
        const Blueprint& blueprint, 
        const glm::ivec2& startPosition,
        const glm::ivec2& target,
        int weaponBaseDamage
    ) {
        return std::make_shared<Projectile>(
            grid,
            blueprint.textureId,
            ownerId,
            startPosition, 
            target, 
            blueprint.stats, 
            weaponBaseDamage, 
            blueprint.onHitCallback
        );
    }

    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);
    void update(uint32_t timeSinceLastFrame);

    bool hasReachedTarget(void) const;
};