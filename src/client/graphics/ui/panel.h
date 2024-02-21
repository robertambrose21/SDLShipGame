#pragma once

#include "core/glmimport.h"
#include "../sdlimport.h"
#include "../graphicscontext.h"

// TODO: Delete probably
class Panel {
private:
    glm::ivec2 position;
    glm::ivec2 size;

    uint8_t r, g, b, a;

public:
    Panel(const glm::ivec2& position, const glm::ivec2& size);

    void draw(SDL_Renderer* renderer);

    void setPosition(const glm::ivec2& position);
    glm::ivec2 getPosition(void) const;

    void setSize(const glm::ivec2& size);
    glm::ivec2 getSize(void) const;

    void setColour(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
};