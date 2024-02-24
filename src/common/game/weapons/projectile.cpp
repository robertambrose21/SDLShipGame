#include "projectile.h"
#include "game/application/application.h"

Projectile::Projectile(
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
    std::function<void(int, const glm::ivec2&, int, bool)> onHitCallback
) :
    grid(grid),
    entityPool(entityPool),
    publisher(publisher),
    textureId(textureId),
    ownerId(ownerId),
    startPosition(startPosition),
    position(startPosition),
    target(target),
    stats(stats),
    damageSource(damageSource),
    isAnimationOnly(isAnimationOnly),
    onHitCallback(onHitCallback),
    timeSinceLive(0.0f)
{
    distanceToTarget = glm::distance(glm::vec2(target), glm::vec2(startPosition));
}

void Projectile::update(int64_t timeSinceLastFrame) {
    timeSinceLive += timeSinceLastFrame;

    step = std::min(1.0f, calculateStep());
    position = glm::lerp(startPosition, target, step);

    if(hasReachedTarget()) {
        doHit(target);
    }
}

bool Projectile::hasReachedTarget(void) {
    return step >= 1.0f;
}

int Projectile::getOwnerId(void) const {
    return ownerId;
}

Projectile::Stats Projectile::getStats(void) const {
    return stats;
}

void Projectile::doHit(const glm::ivec2& position) {
    onHitCallback(ownerId, position, 1, isAnimationOnly);

    if(!isAnimationOnly) {
        apply(position);
    }
}

void Projectile::apply(const glm::ivec2& position) {
    auto entity = Entity::filterByTile(
        position.x,
        position.y,
        entityPool->getEntities(),
        ownerId);

    int damage = 0;

    if (entity != nullptr) {
        damage = damageSource.apply(entity);

        // Effects could be applied from damage source too?
        for (auto const& effect : stats.effects) {
            if (effect.name == "freeze") {
                entity->setFrozenFor(effect.duration);
            }
        }
    }
    // TODO: This is just an onHitCallback
    else {
        for (auto const& effect : stats.effects) {
            if (effect.name == "freeze") {
                glm::ivec2 dir = startPosition - target;
                auto perp = glm::normalize(glm::vec2(dir.y, -dir.x));
                auto pX = std::min(grid->getWidth() - 1, (int)std::round(perp.x));
                auto pY = std::min(grid->getHeight() - 1, (int)std::round(perp.y));

                // TODO: Colour/unfreeze tiles after some time
                grid->setTileFrozenFor(position.x, position.y, effect.duration);
                grid->setTileFrozenFor(position.x + pX, position.y + pY, effect.duration);
                grid->setTileFrozenFor(position.x - pX, position.y - pY, effect.duration);
            }
        }
    }

    publisher.publish<ProjectileEventData>({this, entity, position, damage});
}

float Projectile::calculateStep(void) const {
    return ((timeSinceLive / 1000.0f) * stats.speed) / distanceToTarget;
}

uint32_t Projectile::getTextureId(void) const {
    return textureId;
}

glm::ivec2 Projectile::getPosition(void) const {
    return position;
}
