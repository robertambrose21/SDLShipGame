#include "projectile.h"
#include "game/application/application.h"

Projectile::Projectile(
    Grid& grid,
    uint32_t textureId,
    int ownerId,
    const glm::ivec2& startPosition,
    const glm::ivec2& target,
    const Stats& stats,
    int weaponBaseDamage,
    std::function<void(Grid&, int, const glm::ivec2&, int)> onHitCallback
) :
    grid(grid),
    textureId(textureId),
    ownerId(ownerId),
    startPosition(startPosition),
    target(target),
    stats(stats),
    weaponBaseDamage(weaponBaseDamage),
    onHitCallback(onHitCallback),
    timeSinceLive(0.0f)
{
    distanceToTarget = glm::distance(glm::vec2(target), glm::vec2(startPosition));
}

void Projectile::draw(GraphicsContext& graphicsContext) {
    graphicsContext.getGridRenderer().draw(graphicsContext, textureId, position);
}

void Projectile::update(uint32_t timeSinceLastFrame) {
    timeSinceLive += timeSinceLastFrame;

    step = std::min(1.0f, calculateStep());
    position = glm::lerp(startPosition, target, step);

    if(hasReachedTarget()) {
        doHit(target);
    }
}

bool Projectile::hasReachedTarget(void) const {
    return step >= 1.0f;
}

void Projectile::doHit(const glm::ivec2& position) {
    auto entity = Entity::filterByTile(
        position.x, 
        position.y, 
        Application::getContext().getEntityPool().getEntities(), 
        ownerId
    );

    onHitCallback(grid, ownerId, position, 1);

    if(entity != nullptr) {
        auto damage = std::floor(weaponBaseDamage * stats.damageMultiplier);

        entity->takeDamage(damage);
        for(auto const& effect : stats.effects) {
            if(effect.name == "freeze") {
                entity->setFrozenFor(effect.duration);
            }
        }

        std::cout 
            << entity->getName()
            << "#"
            << entity->getId()
            << " was hit by a projectile from participant ["
            << ownerId
            << "] and took "
            << damage
            << " damage! "
            << entity->getName()
            << "#"
            << entity->getId()
            << " now has "
            << entity->getCurrentHP()
            << " HP."
            << std::endl;
    }
    // TODO: This is just an onHitCallback
    else {
        for(auto const& effect : stats.effects) {
            if(effect.name == "freeze") {
                glm::ivec2 dir = startPosition - target;
                auto perp = glm::normalize(glm::vec2(dir.y, -dir.x));
                auto pX = std::min(grid.getWidth() - 1, (int) std::round(perp.x));
                auto pY = std::min(grid.getHeight() - 1, (int) std::round(perp.y));

                // TODO: Colour/unfreeze tiles after some time
                grid.setTileFrozenFor(position.x, position.y, effect.duration);
                grid.setTileFrozenFor(position.x + pX, position.y + pY, effect.duration);
                grid.setTileFrozenFor(position.x - pX, position.y - pY, effect.duration);
            }
        }
    }
}

float Projectile::calculateStep(void) const {
    return ((timeSinceLive / 1000.0f) * stats.speed) / distanceToTarget;
}
