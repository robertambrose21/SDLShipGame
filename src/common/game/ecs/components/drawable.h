// TODO: Change me to "components.h"
#pragma once

#include <cstdint>

#include "core/glmimport.h"

struct Colour {
    uint8_t r, g, b, a;
};

struct Drawable {
    uint32_t textureId;
    uint32_t selectedTextureId;
    Colour colour;
};

typedef glm::ivec2 Position;