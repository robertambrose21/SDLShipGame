#include "text.h"

Text::Text(const std::string& path, const glm::ivec2& position) :
    message(""),
    position(position)
{
    loadFromFile(path);
}

void Text::loadFromFile(const std::string& path) {
    font = std::unique_ptr<TTF_Font, ttf_deleter>(TTF_OpenFont(path.c_str(), 28), ttf_deleter());
}

void Text::draw(const std::shared_ptr<SDL_Renderer>& renderer, const std::string& message) {
    if(this->message != message) {
        setMessage(renderer, message);
    }

    SDL_Rect rect = { position.x, position.y, dimensions.x, dimensions.y };
    SDL_RenderCopy(renderer.get(), texture.get(), NULL, &rect);
}

void Text::setMessage(const std::shared_ptr<SDL_Renderer>& renderer, const std::string& message) {
    this->message = message;

    SDL_Surface* surface = TTF_RenderText_Solid(font.get(), message.c_str(), { 0xFF, 0x00, 0x00, 0xFF});

    if(surface == NULL) {
        std::cout << "Failed to set message \"" << message << "\":" << TTF_GetError() << std::endl;
        return;
    }

    texture = std::unique_ptr<SDL_Texture, sdl_deleter>(SDL_CreateTextureFromSurface(renderer.get(), surface), sdl_deleter());

    if(texture == NULL) {
        std::cout << "Failed to create texture from message \"" << message << "\":" << SDL_GetError() << std::endl;
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