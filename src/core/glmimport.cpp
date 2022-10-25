#include "glmimport.h"

namespace glm {

    vec2 lerp(vec2 x, vec2 y, float t) {
        return x * (1 - t) + y * t;
    }

}