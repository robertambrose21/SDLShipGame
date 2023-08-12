#include "healthbar.h"

HealthBar::HealthBar(int totalHP) :
    totalHP(totalHP)
{ }

void HealthBar::draw(GraphicsContext& graphicsContext, const glm::ivec2& position, int currentHP) {
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