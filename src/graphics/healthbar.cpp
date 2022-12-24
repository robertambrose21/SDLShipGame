#include "healthbar.h"

HealthBar::HealthBar(int totalHP) :
    totalHP(totalHP)
{ }

void HealthBar::draw(const std::shared_ptr<GraphicsContext>& graphicsContext, const glm::ivec2& position, int currentHP) {
    if(totalHP == currentHP) {
        return;
    }

    auto renderer = graphicsContext->getRenderer().get();
    auto gridRenderer = graphicsContext->getGridRenderer();

    auto const &realPosition = gridRenderer->getTilePosition(position.x, position.y);
    auto const &width = gridRenderer->getTileSize();

    int hpLeftWidth = (currentHP / (float)totalHP) * width;

    SDL_Rect total = { realPosition.x, realPosition.y, width, 5 };
    SDL_Rect hpLeft = { realPosition.x, realPosition.y, hpLeftWidth, 5 };

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &total);
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &hpLeft);
}