#include "text.h"

Text::Text(
    std::unique_ptr<Font> font, 
    const glm::ivec2& position, 
    const glm::ivec2& maxDimensions,
    const SDL_Color& colour
) :
    font(std::move(font)),
    message(""),
    position(position),
    maxDimensions(maxDimensions),
    colour(colour)
{
    game_assert(maxDimensions.x > 0);
    game_assert(maxDimensions.y > 0);
}

void Text::draw(SDL_Renderer* renderer, const std::string& message) {
    if(this->message != message) {
        setMessage(renderer, message);
    }

    int scroll = std::max(0, dimensions.y - maxDimensions.y);

    SDL_Rect rect = { 
        position.x, 
        position.y, 
        std::min(dimensions.x, maxDimensions.x),
        std::min(dimensions.y, maxDimensions.y)
    };
    SDL_Rect clip = { 0, scroll, maxDimensions.x, maxDimensions.y };
    SDL_RenderCopy(renderer, texture.get(), &clip, &rect);
}

void Text::setMessage(SDL_Renderer* renderer, const std::string& message) {
    this->message = message;

    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(
        font->font.get(), message.c_str(), colour, maxDimensions.x);

    if(surface == NULL) {
        std::cout << "Failed to set message \"" << message << "\": " << TTF_GetError() << std::endl;
        return;
    }

    texture = std::unique_ptr<SDL_Texture, sdl_deleter>(SDL_CreateTextureFromSurface(renderer, surface), sdl_deleter());

    if(texture == NULL) {
        std::cout << "Failed to create texture from message \"" << message << "\": " << SDL_GetError() << std::endl;
    }
    else {
        dimensions.x = surface->w;
        dimensions.y = surface->h;
    }

    SDL_FreeSurface(surface);
}


void Text::setPosition(const glm::ivec2& position) {
    this->position = position;
}

glm::ivec2 Text::getPosition(void) const {
    return position;
}

glm::ivec2 Text::getDimensions(void) const {
    return dimensions;
}

void Text::setColour(const SDL_Color& colour) {
    this->colour = colour;
}

SDL_Color Text::getColour(void) const {
    return colour;
}