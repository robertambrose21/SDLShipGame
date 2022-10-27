#include "projectile.h"

Projectile::Projectile(
    std::shared_ptr<Grid> grid,
    std::shared_ptr<Texture> texture,
    glm::ivec2 startPosition,
    std::shared_ptr<Entity> target,
    Stats stats,
    int weaponBaseDamage,
    std::function<void(std::shared_ptr<Grid>, std::shared_ptr<Entity>, int)> onHitCallback
) :
    grid(grid),
    texture(texture),
    startPosition(startPosition),
    target(target),
    stats(stats),
    weaponBaseDamage(weaponBaseDamage),
    onHitCallback(onHitCallback),
    timeSinceLive(0.0f)
{
    distanceToTarget = glm::distance(glm::vec2(target->getPosition()), glm::vec2(startPosition));
}

void Projectile::draw(std::shared_ptr<SDL_Renderer> renderer, std::shared_ptr<GridRenderer> gridRenderer) {
    auto realPosition = gridRenderer->getTilePosition(position.x, position.y);
    SDL_Rect dst = { realPosition.x, realPosition.y, gridRenderer->getTileSize(), gridRenderer->getTileSize() };
    texture->draw(renderer, NULL, &dst);
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
