#pragma once

#include "../core/glmimport.h"
#include "../graphics/gridrenderer.h"

class AreaOfEffect {
public:
    typedef struct _stats {
        float radius;
        int turns;
    } Stats;

private:
    std::shared_ptr<GridRenderer> gridRenderer;
    std::shared_ptr<Texture> texture;

    std::vector<glm::ivec2> effectedTilePositions;

    Stats stats;

public:
    AreaOfEffect(
        std::shared_ptr<GridRenderer> gridRenderer, 
        std::shared_ptr<Texture> texture, 
        glm::ivec2 position, 
        Stats stats
    );

    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);
};