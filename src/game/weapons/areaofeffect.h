#pragma once

#include "core/glmimport.h"
#include "graphics/gridrenderer.h"

class AreaOfEffect {
public:
    typedef struct _stats {
        float radius;
        int turns;
    } Stats;

private:
    std::shared_ptr<Grid> grid;
    std::shared_ptr<Texture> texture;

    std::vector<glm::ivec2> effectedTilePositions;

    int liveTurn;
    glm::ivec2 position;
    Stats stats;

public:
    AreaOfEffect(
        std::shared_ptr<Grid> grid, 
        std::shared_ptr<Texture> texture,
        int liveTurn,
        glm::ivec2 position, 
        Stats stats
    );

    void draw(std::shared_ptr<SDL_Renderer> renderer, std::shared_ptr<GridRenderer> gridRenderer);
    void update(const Uint32& timeSinceLastFrame);

    Stats getStats(void) const;
};