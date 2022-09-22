#include "projectile.h"

// TODO: Move elsewhere
glm::vec2 lerp(glm::vec2 x, glm::vec2 y, float t) {
    return x * (1 - t) + y * t;
}

Projectile::Projectile(
    std::shared_ptr<GridRenderer> grid,
    std::shared_ptr<Texture> texture,
    glm::ivec2 startPosition,
    std::shared_ptr<Entity> target,
    Stats stats,
    int weaponBaseDamage
) :
    grid(grid),
    texture(texture),
    startPosition(startPosition),
    target(target),
    stats(stats),
    weaponBaseDamage(weaponBaseDamage),
    step(0.0f)
{ }

void Projectile::draw(std::shared_ptr<SDL_Renderer> renderer) {
    auto realPosition = grid->getTilePosition(position.x, position.y);
    SDL_Rect dst = { realPosition.x, realPosition.y, grid->getTileSize(), grid->getTileSize() };
    texture->draw(renderer, NULL, &dst);
}

void Projectile::update(const Uint32& timeSinceLastFrame) {
    step += timeSinceLastFrame;

    position = lerp(startPosition, target->getPosition(), (step / 1000.0f) * stats.speed);

    if(hasReachedTarget()) {
        target->takeDamage((float) weaponBaseDamage * stats.damageMultiplier);
    }
}

bool Projectile::hasReachedTarget(void) const {
    return (step / 1000.0f) * stats.speed >= 1.0f;
}
