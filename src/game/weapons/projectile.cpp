#include "projectile.h"

Projectile::Projectile(
    std::shared_ptr<Grid> grid,
    uint8_t textureId,
    glm::ivec2 startPosition,
    std::shared_ptr<Entity> target,
    Stats stats,
    int weaponBaseDamage,
    std::function<void(std::shared_ptr<Grid>, std::shared_ptr<Entity>, int)> onHitCallback
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

void Projectile::draw(std::shared_ptr<GraphicsContext> graphicsContext) {
    graphicsContext->getGridRenderer()->draw(graphicsContext, textureId, position);
}

void Projectile::update(const Uint32& timeSinceLastFrame) {
    timeSinceLive += timeSinceLastFrame;

    position = glm::lerp(startPosition, target->getPosition(), getStep());

    if(hasReachedTarget()) {
        target->takeDamage((float) weaponBaseDamage * stats.damageMultiplier);
        onHitCallback(grid, target, 1);
    }
}

bool Projectile::hasReachedTarget(void) const {
    return getStep() >= 1.0f;
}

float Projectile::getStep(void) const {
    return ((timeSinceLive / 1000.0f) * stats.speed) / distanceToTarget;
}
