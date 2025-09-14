#pragma once

#include <cstdint>
#include <map>
#include <deque>
#include <memory>

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

struct PositionDirty { };

// TODO: Make this UUID eventually
typedef uint32_t ExternalId;

class Action;

struct ActionChain {
    std::map<int, std::deque<std::unique_ptr<Action>>> chain;

    ActionChain() = default;
    ActionChain(ActionChain&&) noexcept = default;
    ActionChain& operator=(ActionChain&&) noexcept = default;

    ActionChain(const ActionChain&) = delete;
    ActionChain& operator=(const ActionChain&) = delete;
};

// Temp, decompose weapons into pure components without any inheritance
class Weapon;

struct WeaponHolder {
    std::unique_ptr<Weapon> weapon;
};
