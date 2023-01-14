#include "button.h"

Button::Button(uint32_t textureId, const glm::ivec2& position, const glm::ivec2& size) :
    textureId(textureId),
    position(position),
    size(size),
    onClickFunc([]{})
{ }

void Button::draw(const std::shared_ptr<GraphicsContext>& graphicsContext) {
    SDL_Rect dst = {
        position.x,
        position.y,
        size.x,
        size.y
    };

    graphicsContext->getTextureLoader()
        ->loadTexture(textureId)
        ->draw(graphicsContext->getRenderer(), NULL, &dst);
}

void Button::handleClickEvent(int mouseX, int mouseY) {
    if(!isInBounds(mouseX, mouseY)) {
        return;
    }

    onClickFunc();
}

void Button::onClick(std::function<void()>&& onClickFunc) {
    this->onClickFunc = onClickFunc;
}

bool Button::isInBounds(int x, int y) {
    if(x < position.x || x > position.x + size.x) {
        return false;
    }

    if(y < position.y || y > position.y + size.y) {
        return false;
    }

    return true;
}