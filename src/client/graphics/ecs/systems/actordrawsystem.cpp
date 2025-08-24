#include "actordrawsytem.h"

ActorDrawSystem::ActorDrawSystem(const std::string& name) :
    DrawSystem(name)
{ }

// TODO: get rid of Actor view
void ActorDrawSystem::update(entt::registry& registry, GraphicsContext& graphicsContext, int64_t timeSinceLastFrame) {
    for(auto [_, drawable, actor]: registry.view<Drawable, Actor>().each()) {
        auto& gridRenderer = graphicsContext.getGridRenderer();

        gridRenderer.draw(
            graphicsContext,
            drawable.textureId,
            { drawable.colour.r, drawable.colour.g, drawable.colour.b },
            drawable.colour.a,
            actor.getPosition()
        );

        if(actor.isSelected()) {
            gridRenderer.draw(graphicsContext, drawable.selectedTextureId, actor.getPosition());
        }
    }

    // for(auto& weapon : drawable->getWeapons()) {
    //     weaponDrawStrategy->draw(weapon, graphicsContext);
    // }

    // if(drawable->getIsFrozen()) {
    //     auto const &realPosition = gridRenderer.getTilePosition(position.x, position.y) 
    //             + gridRenderer.getCamera().getPosition();
    //     auto const &size = gridRenderer.getTileSize();

    //     SDL_Rect frozen = { realPosition.x, realPosition.y, size, size };

    //     SDL_SetRenderDrawBlendMode(graphicsContext.getRenderer(), SDL_BLENDMODE_BLEND);
    //     SDL_SetRenderDrawColor(graphicsContext.getRenderer(), 0x00, 0xFF, 0xFF, 0x7F);
    //     SDL_RenderFillRect(graphicsContext.getRenderer(), &frozen);
    // }
}