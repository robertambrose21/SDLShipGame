#include "projectile.h"
#include "game/application/application.h"

Projectile::Projectile(
    ApplicationContext* context,
    EventPublisher<ProjectileEventData>& publisher,
    uint32_t textureId,
    int ownerId,
    const glm::ivec2& startPosition,
    const glm::ivec2& target,
    const ProjectileStats2& stats,
    const DamageSource& damageSource,
    bool isAnimationOnly,
    std::function<void(int, const glm::ivec2&, int, bool)> onHitCallback
) :
    context(context),
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

ProjectileStats2 Projectile::getStats(void) const {
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
        context->getEntityPool()->getEntities(),
        ownerId);

    int damage = 0;

    if (entity != nullptr) {
        damage = damageSource.apply(entity);

        for (auto& effect : stats.effects) {
            switch(effect.type) {
            case FREEZE:
                context->getEffectController()->addEffect(std::make_unique<FreezeEffect>(entity, effect));
                break;

            case POISON: {
                context->getEffectController()->addEffect(std::make_unique<PoisonEffect>(entity, effect));
                break;
            }

            default:
                break;
            }
        }
    }
    else {
        auto grid = context->getGrid();

        for (auto const& effect : stats.effects) {
            switch(effect.type) {
            case FREEZE: {
                glm::ivec2 dir = startPosition - target;
                auto perp = glm::normalize(glm::vec2(dir.y, -dir.x));
                auto pX = std::min(grid->getWidth() - 1, (int)std::round(perp.x));
                auto pY = std::min(grid->getHeight() - 1, (int)std::round(perp.y));

                context->getEffectController()->addGridEffect(
                    std::make_unique<GridFreezeEffect>(grid, position.x, position.y, effect.duration));
                context->getEffectController()->addGridEffect(
                    std::make_unique<GridFreezeEffect>(grid, position.x + pX, position.y + pY, effect.duration));
                context->getEffectController()->addGridEffect(
                    std::make_unique<GridFreezeEffect>(grid, position.x - pX, position.y - pY, effect.duration));

                break;
            }

            default:
                break;
            }
        }

    }

    publisher.publish<ProjectileEventData>({ this, entity, position, damage });
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
