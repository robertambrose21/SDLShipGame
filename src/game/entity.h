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
    std::deque<glm::ivec2> path;
    Uint32 timeSinceLastMoved;

    Stats stats;

    std::string name;
    int movesLeft;

protected:
    std::shared_ptr<GridRenderer> grid;

    virtual void additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) = 0;

public:
    const Uint32 MOVES_PER_SECOND = 5;

    Entity(
        std::shared_ptr<GridRenderer> grid,
        const std::string& name,
        Stats stats
    );

    void update(const Uint32& timeSinceLastFrame, bool& quit);

    void setTexture(std::shared_ptr<Texture> texture);
    void draw(std::shared_ptr<SDL_Renderer> renderer);

    Stats getStats(void) const;
    const float getSpeed(void);
    void doDamage(const int& amount);

    glm::ivec2 getPosition(void) const;
    void setPosition(const glm::ivec2& position);
    void setPath(std::deque<glm::ivec2> path);

    int getMovesLeft(void) const;
    void useMoves(const int& numMoves);

    void nextTurn(void);
};
