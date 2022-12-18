#pragma once

#include <glm/glm.hpp>

// Fix using glm vec2s as map keys
namespace glm {
    template <typename T, precision P>
    bool operator<(const tvec2<T, P>& a,const tvec2<T, P>& b) {
        return (a.x < b.x || (a.x == b.x && a.y < b.y));
    }

    vec2 lerp(const vec2& x, const vec2& y, float t);
}