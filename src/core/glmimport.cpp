#include "glmimport.h"

namespace glm {

    vec2 lerp(const vec2& x, const vec2& y, const float& t) {
        return x * (1 - t) + y * t;
    }

}