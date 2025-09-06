#include "actordrawsystem.h"

ActorDrawSystem::ActorDrawSystem(const std::string& name) :
    DrawSystem(name)
{ }

void ActorDrawSystem::draw(entt::registry& registry, GraphicsContext& graphicsContext) {
    auto& gridRenderer = graphicsContext.getGridRenderer();

    for(auto [_, drawable, actor, stats, position]: registry.view<Drawable, Actor, Stats::ActorStats, Position>().each()) {
        gridRenderer.draw(
            graphicsContext,
            drawable.textureId,
            { drawable.colour.r, drawable.colour.g, drawable.colour.b },
            drawable.colour.a,
            position
        );

        if(actor.isSelected()) {
            gridRenderer.draw(graphicsContext, drawable.selectedTextureId, position);
        }

        if(actor.getIsFrozen()) {
            auto const &realPosition = gridRenderer.getTilePosition(position.x, position.y) 
                    + gridRenderer.getCamera().getPosition();
            auto const &size = gridRenderer.getTileSize();

            SDL_Rect frozen = { realPosition.x, realPosition.y, size, size };

            SDL_SetRenderDrawBlendMode(graphicsContext.getRenderer(), SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(graphicsContext.getRenderer(), 0x00, 0xFF, 0xFF, 0x7F);
            SDL_RenderFillRect(graphicsContext.getRenderer(), &frozen);
        }

        drawHealthBar(graphicsContext, position, stats.totalHp, stats.hp);
    }
}

void ActorDrawSystem::drawHealthBar(
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