#include "button.h"

Button::Button(uint32_t textureId, const glm::ivec2& position, const glm::ivec2& size) :
    textureId(textureId),
    position(position),
    size(size),
    onClickFunc([]{}),
    isDisabled(false)
{ }

void Button::draw(GraphicsContext& graphicsContext) {
    SDL_Rect dst = {
        position.x,
        position.y,
        size.x,
        size.y
    };

    auto colour = isDisabled ? Texture::Colour { 0x7F, 0x7F, 0x7F } : Texture::Colour { 0xFF, 0xFF, 0xFF };

    graphicsContext.getTextureLoader()
        .loadTexture(textureId)
        ->draw(graphicsContext.getRenderer(), colour, 0xFF, NULL, &dst);
}

bool Button::handleClickEvent(int mouseX, int mouseY) {
    if(isDisabled) {
        return false;
    }

    if(!isInBounds(mouseX, mouseY)) {
        return false;
    }

    click();
    return true;
}

void Button::click(void) {
    onClickFunc();
}

void Button::toggleDisabled(void) {
    isDisabled = !isDisabled;
}

void Button::setDisabled(bool isDisabled) {
    this->isDisabled = isDisabled;
}

bool Button::getIsDisabled(void) const {
    return isDisabled;
}

void Button::setPosition(const glm::ivec2& position) {
    this->position = position;
}

glm::ivec2 Button::getPosition(void) const {
    return position;
}

void Button::setSize(const glm::ivec2& size) {
    this->size = size;
}

glm::ivec2 Button::getSize(void) const {
    return size;
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