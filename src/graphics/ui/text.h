#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "core/glmimport.h"
#include "../sdlimport.h"

class Text {
private:
    struct ttf_deleter {
        void operator()(TTF_Font *p) const { TTF_CloseFont(p); }
    };

    struct sdl_deleter {
        void operator()(SDL_Texture *p) const { SDL_DestroyTexture(p); }
    };

    std::unique_ptr<TTF_Font, ttf_deleter> font;
    std::unique_ptr<SDL_Texture, sdl_deleter> texture;

    std::string message;
    glm::ivec2 position;
    glm::ivec2 dimensions;

    void loadFromFile(const std::string& path);
    void setMessage(const std::shared_ptr<SDL_Renderer>& renderer, const std::string& message);

public:
    Text(const std::string& path, const glm::ivec2& position);

    void draw(const std::shared_ptr<SDL_Renderer>& renderer, const std::string& message);

    void setPosition(const glm::ivec2& position);
    glm::ivec2 getPosition(void) const;
};