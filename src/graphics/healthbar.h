#pragma once

#include "core/glmimport.h"
#include "sdlimport.h"
#include "graphicscontext.h"

class HealthBar {
private:
    int totalHP;
    
public:
    HealthBar(int totalHP);

    void draw(
        const std::shared_ptr<GraphicsContext>& graphicsContext, 
        const glm::ivec2& position, 
        int currentHP
    );
};