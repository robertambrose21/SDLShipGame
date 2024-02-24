#include "projectile.h"
#include "game/application/application.h"

Projectile::Projectile(
    ApplicationContext* context,
    EventPublisher<ProjectileEventData>& publisher,
    uint32_t textureId,
    int ownerId,
    const glm::ivec2& startPosition,
    const glm::ivec2& target,
    const ProjectileStats& stats,
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

ProjectileStats Projectile::getStats(void) const {
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
                entity->addEffect(std::make_unique<FreezeEffect>(context, effect.duration));
                break;

            case POISON: {
                auto damageTicks = std::vector<int>(effect.duration);
                std::fill(damageTicks.begin(), damageTicks.end(), effect.damagePerTick);
                entity->addEffect(std::make_unique<PoisonEffect>(context, damageTicks));
                break;
            }

            default:
                break;
            }
            // entity->addEffect();
        }

        // // Effects could be applied from damage source too?
        // for (auto const& effect : stats.effects) {
        //     if (effect.name == "freeze") {
        //         entity->setFrozenFor(effect.duration);
        //     }
        // }
    }
    // TODO: This is just an onHitCallback
    else {
        // for (auto const& effect : stats.effects) {
        //     if (effect.name == "freeze") {
        //         glm::ivec2 dir = startPosition - target;
        //         auto perp = glm::normalize(glm::vec2(dir.y, -dir.x));
        //         auto pX = std::min(grid->getWidth() - 1, (int)std::round(perp.x));
        //         auto pY = std::min(grid->getHeight() - 1, (int)std::round(perp.y));

        //         // TODO: Colour/unfreeze tiles after some time
        //         grid->setTileFrozenFor(position.x, position.y, effect.duration);
        //         grid->setTileFrozenFor(position.x + pX, position.y + pY, effect.duration);
        //         grid->setTileFrozenFor(position.x - pX, position.y - pY, effect.duration);
        //     }
        // }
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
