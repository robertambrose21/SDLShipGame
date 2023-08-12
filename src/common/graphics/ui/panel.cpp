#include "panel.h"

Panel::Panel(const glm::ivec2& position, const glm::ivec2& size) :
    position(position),
    size(size),
    r(0xFF),
    g(0xFF),
    b(0xFF),
    a(0xFF)
{ }

void Panel::draw(SDL_Renderer* renderer) {
    SDL_Rect rect = { position.x, position.y, size.x, size.y };
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
}

void Panel::setPosition(const glm::ivec2& position) {
    this->position = position;
}

glm::ivec2 Panel::getPosition(void) const {
    return position;
}

void Panel::setSize(const glm::ivec2& size) {
    this->size = size;
}

glm::ivec2 Panel::getSize(void) const {
    return size;
}

void Panel::setColour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}