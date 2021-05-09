#pragma once

#include <glm/glm.hpp>
#include <string>

#include "../graphics/gridrenderer.h"

class Entity {
public:
    typedef struct _stats {
        int movesPerTurn;
        int hp;
    } Stats;

private:
    std::shared_ptr<Texture> texture;

    glm::ivec2 position;

    Stats stats;

    std::string name;

protected:
    std::shared_ptr<GridRenderer> grid;

public:

    const Uint32 MOVES_PER_SECOND = 5;

    Entity(std::shared_ptr<GridRenderer> grid, const std::string& name, Stats stats);

    void setTexture(std::shared_ptr<Texture> texture);
    void draw(std::shared_ptr<SDL_Renderer> renderer);

    virtual void update(const Uint32& timeSinceLastFrame, bool& quit) = 0;

    Stats getStats(void) const;
    const float getSpeed(void);

    glm::ivec2 getPosition(void) const;
    void setPosition(const glm::ivec2& position);
};
