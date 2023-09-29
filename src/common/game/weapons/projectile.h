#pragma once

#include <functional>
#include <algorithm>
#include <vector>

#include "core/glmimport.h"
// #include "graphics/gridrenderer.h"
#include "game/entities/entity.h"
#include "game/effects/effect.h"
#include "core/event/eventpublisher.h"

class Entity;
class Projectile;

struct ProjectileEventData {
    Projectile* projectile;
    Entity* target;
    glm::ivec2 hitPosition;
    int damage;
};

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
        std::function<void(Grid&, int, const glm::ivec2&, int)> onHitCallback;

        _blueprint(
            const Stats& stats,
            const std::string& name,
            uint32_t textureId,
            std::function<void(Grid&, int, const glm::ivec2&, int)> onHitCallback =
                [](Grid&, int, const glm::ivec2&, int){ }
        ) :
            stats(stats),
            name(name),
            textureId(textureId),
            onHitCallback(onHitCallback)
        { }
    } Blueprint;

private:
    Stats stats;

    Grid& grid;
    EventPublisher<ProjectileEventData>& publisher;
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
    std::function<void(Grid&, int, const glm::ivec2&, int)> onHitCallback;

    float calculateStep(void) const;
    void doHit(const glm::ivec2& position);

public:
    Projectile(
        Grid& grid,
        EventPublisher<ProjectileEventData>& publisher,
        uint32_t textureId,
        int ownerId,
        const glm::ivec2& startPosition,
        const glm::ivec2& target,
        const Stats& stats,
        int weaponBaseDamage,
        std::function<void(Grid&, int, const glm::ivec2&, int)> onHitCallback = 
            [](Grid&, int, const glm::ivec2&, int){ }
    );

    static std::unique_ptr<Projectile> create(
        Grid& grid,
        EventPublisher<ProjectileEventData>& publisher,
        int ownerId,
        const Blueprint& blueprint, 
        const glm::ivec2& startPosition,
        const glm::ivec2& target,
        int weaponBaseDamage
    ) {
        return std::make_unique<Projectile>(
            grid,
            publisher,
            blueprint.textureId,
            ownerId,
            startPosition, 
            target, 
            blueprint.stats, 
            weaponBaseDamage, 
            blueprint.onHitCallback
        );
    }

    // void draw(GraphicsContext& graphicsContext);
    void update(uint32_t timeSinceLastFrame);

    bool hasReachedTarget(void);

    int getOwnerId(void) const;
    Stats getStats(void) const;

    uint32_t getTextureId(void) const;
    glm::ivec2 getPosition(void) const;
};