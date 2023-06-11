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
    std::function<void(const std::shared_ptr<Grid>&, int, const std::shared_ptr<Entity>&, int)> onHitCallback
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

    position = glm::lerp(startPosition, target, getStep());

    auto entity = Entity::filterByTile(
        position.x, 
        position.y, 
        Application::getContext()->getEntityPool()->getEntities(), 
        ownerId
    );

    if(hasReachedTarget()) {
        if(entity != nullptr) {
            entity->takeDamage((float) weaponBaseDamage * stats.damageMultiplier);
            onHitCallback(grid, ownerId, entity, 1);

            for(auto const& effect : stats.effects) {
                if(effect.name == "freeze") {
                    entity->setFrozenFor(effect.duration);
                }
            }
        }
        else {
            for(auto const& effect : stats.effects) {
                if(effect.name == "freeze") {
                    // TODO: Colour/unfreeze tiles after some time
                    grid->setTileWalkable(position.x, position.y, false);
                }
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
