#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "core/glmimport.h"
#include "core/util/gameassert.h"
#include "../sdlimport.h"

struct Font {
    struct ttf_deleter {
        void operator()(TTF_Font *p) const { TTF_CloseFont(p); }
    };

    std::unique_ptr<TTF_Font, ttf_deleter> font;

    static std::unique_ptr<Font> loadFromFile(const std::string& path, int ptsize) {
        Font font;
        font.font = std::unique_ptr<TTF_Font, ttf_deleter>(TTF_OpenFont(path.c_str(), ptsize), ttf_deleter());
        return std::make_unique<Font>(std::move(font));
    }
};

class Text {
private:
    struct sdl_deleter {
        void operator()(SDL_Texture *p) const { SDL_DestroyTexture(p); }
    };

    std::unique_ptr<Font> font;
    std::unique_ptr<SDL_Texture, sdl_deleter> texture;

    std::string message;
    glm::ivec2 position;
    glm::ivec2 dimensions;
    glm::ivec2 maxDimensions;
    SDL_Color colour;

    void setMessage(SDL_Renderer* renderer, const std::string& message);

public:
    Text(
        std::unique_ptr<Font> font, 
        const glm::ivec2& position, 
        const glm::ivec2& maxDimensions, 
        const SDL_Color& colour = { 0x00, 0x00, 0x00, 0xFF }
    );

    void draw(SDL_Renderer* renderer, const std::string& message);

    void setPosition(const glm::ivec2& position);
    glm::ivec2 getPosition(void) const;
    glm::ivec2 getDimensions(void) const;

    void setColour(const SDL_Color& colour);
    SDL_Color getColour(void) const;
};