#include "areaofeffect.h"

AreaOfEffect::AreaOfEffect(
    std::shared_ptr<Grid> grid,
    std::shared_ptr<Texture> texture,
    int liveTurn,
    glm::ivec2 position, 
    Stats stats
) :
    grid(grid),
    texture(texture),
    liveTurn(liveTurn),
    position(position),
    stats(stats)
{
    effectedTilePositions = grid->getTilesInCircle(position.x, position.y, stats.radius);
}

// TODO: Move this draw stuff to a common function
void AreaOfEffect::draw(std::shared_ptr<SDL_Renderer> renderer, std::shared_ptr<GridRenderer> gridRenderer) {
    for(auto position : effectedTilePositions) {
        auto realPosition = gridRenderer->getTilePosition(position.x, position.y);
        SDL_Rect dst = { realPosition.x, realPosition.y, gridRenderer->getTileSize(), gridRenderer->getTileSize() };
        texture->draw(renderer, NULL, &dst);
    }
}

void AreaOfEffect::update(const Uint32& timeSinceLastFrame) {
    //
}

AreaOfEffect::Stats AreaOfEffect::getStats(void) const {
    return stats;
}