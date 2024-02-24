#pragma once

#include <functional>
#include <algorithm>
#include <vector>

#include "core/glmimport.h"
#include "game/entities/entity.h"
#include "game/effects/effect.h"
#include "core/event/eventpublisher.h"
#include "game/event/events.h"

class Entity;
class Projectile;

class Projectile {
public:
    typedef struct _stats {
        float speed;
        std::vector<Effect> effects;
    } Stats;

    typedef struct _blueprint {
        Stats stats;
        std::string name;
        uint32_t textureId;
        std::function<void(Grid*, int, const glm::ivec2&, int, bool)> onHitCallback;

        _blueprint(
            const Stats& stats,
            const std::string& name,
            uint32_t textureId,
            std::function<void(Grid*, int, const glm::ivec2&, int, bool)> onHitCallback =
                [](Grid*, int, const glm::ivec2&, int, bool){ }
        ) :
            stats(stats),
            name(name),
            textureId(textureId),
            onHitCallback(onHitCallback)
        { }
    } Blueprint;

private:
    Stats stats;

    Grid* grid;
    EntityPool* entityPool;
    EventPublisher<ProjectileEventData>& publisher;
    uint32_t textureId;

    glm::ivec2 position;
    glm::ivec2 startPosition;
    glm::ivec2 target;

    int ownerId;

    float timeSinceLive;
    float distanceToTarget;
    float step;
    bool isAnimationOnly;

    DamageSource damageSource;

    std::function<void(Grid*, int, const glm::ivec2&, int, bool)> onHitCallback;

    float calculateStep(void) const;
    void doHit(const glm::ivec2& position);
    void apply(const glm::ivec2 & position);

public:
Projectile(
    Grid* grid,
    EntityPool* entityPool,
    EventPublisher<ProjectileEventData>& publisher,
    uint32_t textureId,
    int ownerId,
    const glm::ivec2& startPosition,
    const glm::ivec2& target,
    const Stats& stats,
    const DamageSource& damageSource,
    bool isAnimationOnly,
    std::function<void(Grid*, int, const glm::ivec2&, int, bool)> onHitCallback =
        [](Grid*, int, const glm::ivec2&, int, bool) {});

static std::unique_ptr<Projectile> create(
    Grid* grid,
    EntityPool* entityPool,
    EventPublisher<ProjectileEventData>& publisher,
    int ownerId,
    const Blueprint& blueprint,
    const glm::ivec2& startPosition,
    const glm::ivec2& target,
    const DamageSource& damageSource,
    bool isAnimationOnly) {
    return std::make_unique<Projectile>(
        grid,
        entityPool,
        publisher,
        blueprint.textureId,
        ownerId,
        startPosition,
        target,
        blueprint.stats,
        damageSource,
        isAnimationOnly,
        blueprint.onHitCallback);
    }

    void update(int64_t timeSinceLastFrame);

    bool hasReachedTarget(void);

    int getOwnerId(void) const;
    Stats getStats(void) const;

    uint32_t getTextureId(void) const;
    glm::ivec2 getPosition(void) const;
};