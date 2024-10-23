#pragma once

#include <glm/glm.hpp>
#include <functional>

namespace glm {
    // Fix using glm vec2s as map keys
    template <typename T, precision P>
    bool operator<(const tvec2<T, P>& a,const tvec2<T, P>& b) {
        return (a.x < b.x || (a.x == b.x && a.y < b.y));
    }

    vec2 lerp(const vec2& x, const vec2& y, float t);

    struct ivec2Hash {
        std::size_t operator()(const ivec2& v) const noexcept {
            std::size_t h1 = std::hash<int>{}(v.x);
            std::size_t h2 = std::hash<int>{}(v.y);
            
            return h1 ^ (h2 << 1);
        }
    };
}
