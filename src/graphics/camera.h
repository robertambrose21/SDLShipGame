#pragma once

#include "core/glmimport.h"

class Camera {
private:
    glm::ivec2 position;

public:
    Camera(const glm::ivec2& position);

    void move(const glm::ivec2& amount);

    void setPosition(const glm::ivec2& position);
    glm::ivec2 getPosition(void) const;
};