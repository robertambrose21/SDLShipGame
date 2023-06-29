#include "projectile.h"
#include "game/application/application.h"

Projectile::Projectile(
    const std::shared_ptr<Grid>& grid,
    uint32_t textureId,
    int ownerId,
    const glm::ivec2& startPosition,
    const glm::ivec2& target,
    const Stats& stats,
    int weaponBaseDamage,
    std::function<void(const std::shared_ptr<Grid>&, int, const glm::ivec2&, int)> onHitCallback
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

void Projectile::draw(const std::shared_ptr<GraphicsContext>& graphicsContext) {
    game_assert(graphicsContext != nullptr);
    graphicsContext->getGridRenderer()->draw(graphicsContext, textureId, position);
}

void Projectile::update(uint32_t timeSinceLastFrame) {
    timeSinceLive += timeSinceLastFrame;

    step = std::min(1.0f, calculateStep());
    position = glm::lerp(startPosition, target, step);

    if(hasReachedTarget()) {
        doHit(position);
    }
}

bool Projectile::hasReachedTarget(void) const {
    return step >= 1.0f;
}

void Projectile::doHit(const glm::ivec2& position) {
    auto entity = Entity::filterByTile(
        position.x, 
        position.y, 
        Application::getContext()->getEntityPool()->getEntities(), 
        ownerId
    );

    onHitCallback(grid, ownerId, position, 1);

    if(entity != nullptr) {
        entity->takeDamage((float) weaponBaseDamage * stats.damageMultiplier);
        for(auto const& effect : stats.effects) {
            if(effect.name == "freeze") {
                entity->setFrozenFor(effect.duration);
            }
        }
    }
    // TODO: This is just an onHitCallback
    else {
        for(auto const& effect : stats.effects) {
            if(effect.name == "freeze") {
                // TODO: Colour/unfreeze tiles after some time
                grid->setTileWalkable(position.x, position.y, false);
            }
        }
    }
}

float Projectile::calculateStep(void) const {
    return ((timeSinceLive / 1000.0f) * stats.speed) / distanceToTarget;
}
