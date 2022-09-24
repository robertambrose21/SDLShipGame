#pragma once

#include "../core/glmimport.h"
#include "../graphics/gridrenderer.h"

class AreaOfEffect {
private:
    std::shared_ptr<GridRenderer> gridRenderer;
    std::shared_ptr<Texture> texture;

    std::vector<glm::ivec2> effectedTilePositions;

public:
    AreaOfEffect(
        std::shared_ptr<GridRenderer> gridRenderer, 
        std::shared_ptr<Texture> texture, 
        glm::ivec2 position, 
        float radius
    );

    void draw(std::shared_ptr<SDL_Renderer> renderer);
};