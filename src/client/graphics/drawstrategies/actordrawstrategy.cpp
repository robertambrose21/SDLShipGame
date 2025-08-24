#include "actordrawstrategy.h"

ActorDrawStrategy::ActorDrawStrategy(WeaponDrawStrategy* weaponDrawStrategy) :
    weaponDrawStrategy(weaponDrawStrategy)
{ }

void ActorDrawStrategy::draw(Actor* drawable, GraphicsContext& graphicsContext) {
    auto& gridRenderer = graphicsContext.getGridRenderer();
    auto colour = drawable->getColour();
    auto position = drawable->getPosition();

    // gridRenderer.draw(
    //     graphicsContext,
    //     drawable->getTextureId(),
    //     { colour.r, colour.g, colour.b },
    //     colour.a,
    //     position
    // );

    // if(drawable->isSelected()) {
    //     gridRenderer.draw(graphicsContext, drawable->getSelectedTextureId(), position);
    // }

    for(auto& weapon : drawable->getWeapons()) {
        weaponDrawStrategy->draw(weapon, graphicsContext);
    }

    if(drawable->getIsFrozen()) {
        auto const &realPosition = gridRenderer.getTilePosition(position.x, position.y) 
                + gridRenderer.getCamera().getPosition();
        auto const &size = gridRenderer.getTileSize();

        SDL_Rect frozen = { realPosition.x, realPosition.y, size, size };

        SDL_SetRenderDrawBlendMode(graphicsContext.getRenderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(graphicsContext.getRenderer(), 0x00, 0xFF, 0xFF, 0x7F);
        SDL_RenderFillRect(graphicsContext.getRenderer(), &frozen);
    }
    
    drawHealthBar(graphicsContext, position, drawable->getStats().totalHp, drawable->getCurrentHP());
}

void ActorDrawStrategy::drawHealthBar(
    GraphicsContext& graphicsContext, 
    const glm::ivec2& position,
    int totalHP,
    int currentHP
) {
    if(totalHP == currentHP) {
        return;
    }

    auto renderer = graphicsContext.getRenderer();
    auto& gridRenderer = graphicsContext.getGridRenderer();
    auto& camera = gridRenderer.getCamera();

    auto const &realPosition = gridRenderer.getTilePosition(position.x, position.y) + camera.getPosition();
    auto const &width = gridRenderer.getTileSize();

    int hpLeftWidth = (currentHP / (float)totalHP) * width;

    SDL_Rect total = { realPosition.x, realPosition.y, width, 5 };
    SDL_Rect hpLeft = { realPosition.x, realPosition.y, hpLeftWidth, 5 };

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &total);
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &hpLeft);
}