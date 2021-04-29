#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <string>

#include "../graphics/grid.h"

class Entity {
private:
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Grid> grid;

    glm::ivec2 position;

    int movesPerTurn;

    std::string name;
public:
    Entity(std::shared_ptr<Grid> grid, const std::string& name, int movesPerTurn = 1);

    void setTexture(std::shared_ptr<Texture> texture);
    void draw(std::shared_ptr<SDL_Renderer> renderer);

    virtual void handleKeyPress(SDL_Event event) = 0;
    virtual void update(const Uint32& timeSinceLastFrame, bool& quit) = 0;

    int getMovesPerTurn(void) const;

    glm::ivec2 getPosition(void) const;
    void setPosition(const glm::ivec2& position);
};
