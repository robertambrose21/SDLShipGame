#include "areaofeffect.h"

AreaOfEffect::AreaOfEffect(
    std::shared_ptr<GridRenderer> gridRenderer,
    std::shared_ptr<Texture> texture,
    glm::ivec2 position, 
    Stats stats
) :
    gridRenderer(gridRenderer),
    texture(texture),
    stats(stats)
{
    effectedTilePositions = gridRenderer->getGrid()->getTilesInCircle(position.x, position.y, stats.radius);
}

// TODO: Move this draw stuff to a common function
void AreaOfEffect::draw(std::shared_ptr<SDL_Renderer> renderer) {
    for(auto position : effectedTilePositions) {
        auto realPosition = gridRenderer->getTilePosition(position.x, position.y);
        SDL_Rect dst = { realPosition.x, realPosition.y, gridRenderer->getTileSize(), gridRenderer->getTileSize() };
        texture->draw(renderer, NULL, &dst);
    }
}

void AreaOfEffect::update(const Uint32& timeSinceLastFrame) {
    //
}