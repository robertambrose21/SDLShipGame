#include "actionbutton.h"

ActionButton::ActionButton(
    uint32_t textureId, 
    const glm::ivec2& position, 
    const glm::ivec2& size, 
    int totalUses
) :
    Button(textureId, position, size),
    totalUses(totalUses),
    usesLeft(totalUses)
{ }

void ActionButton::draw(GraphicsContext& graphicsContext) {
    Button::draw(graphicsContext);

    if(getIsDisabled() || usesLeft == totalUses) {
        return;
    }

    auto position = getPosition();
    auto size = getSize();
    int xSize = size.x * ((totalUses - usesLeft) / (float) totalUses);

    SDL_Rect usesLeftRect = { position.x, position.y, xSize, size.y };

    SDL_SetRenderDrawBlendMode(graphicsContext.getRenderer(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(graphicsContext.getRenderer(), 0x00, 0x00, 0x00, 0x7F);
    SDL_RenderFillRect(graphicsContext.getRenderer(), &usesLeftRect);
}

void ActionButton::click(void) {
    if(usesLeft <= 0) {
        return;
    }

    Button::click();
    usesLeft--;

    if(usesLeft == 0) {
        setDisabled(true);
    }
}

void ActionButton::setTotalUses(int totalUses) {
    this->totalUses = totalUses;
}

int ActionButton::getTotalUses(void) const {
    return totalUses;
}

void ActionButton::resetUses(void) {
    usesLeft = totalUses;
}