#include "projectile.h"

Projectile::Projectile(
    const std::shared_ptr<Grid>& grid,
    uint32_t textureId,
    const glm::ivec2& startPosition,
    const std::shared_ptr<Entity>& target,
    const Stats& stats,
    int weaponBaseDamage,
    std::function<void(const std::shared_ptr<Grid>&, const std::shared_ptr<Entity>&, int)> onHitCallback
) :
    grid(grid),
    textureId(textureId),
    startPosition(startPosition),
    target(target),
    stats(stats),
    weaponBaseDamage(weaponBaseDamage),
    onHitCallback(onHitCallback),
    timeSinceLive(0.0f)
{
    distanceToTarget = glm::distance(glm::vec2(target->getPosition()), glm::vec2(startPosition));
}

void Projectile::draw(const std::shared_ptr<GraphicsContext>& graphicsContext) {
    game_assert(graphicsContext != nullptr);
    graphicsContext->getGridRenderer()->draw(graphicsContext, textureId, position);
}

void Projectile::update(uint32_t timeSinceLastFrame) {
    timeSinceLive += timeSinceLastFrame;

    position = glm::lerp(startPosition, target->getPosition(), getStep());

    if(hasReachedTarget()) {
        target->takeDamage((float) weaponBaseDamage * stats.damageMultiplier);
        onHitCallback(grid, target, 1);

        for(auto const& effect : stats.effects) {
            if(effect.name == "freeze") {
                target->setFrozenFor(effect.duration);
            }
        }
    }
}

bool Projectile::hasReachedTarget(void) const {
    return getStep() >= 1.0f;
}

float Projectile::getStep(void) const {
    return ((timeSinceLive / 1000.0f) * stats.speed) / distanceToTarget;
}
