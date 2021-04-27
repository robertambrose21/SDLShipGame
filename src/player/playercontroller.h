#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "../graphics/grid.h"
#include "../graphics/textureloader.h"

class PlayerController {
private:
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Grid> grid;

    glm::ivec2 moveVector;
    glm::ivec2 position;
    glm::vec2 partialPosition;

    int movesPerTurn;

    Uint32 timeSinceLastMoved;
public:
    PlayerController(std::shared_ptr<Grid> grid, int movesPerTurn = 1);

    void setTexture(std::shared_ptr<Texture> texture);
    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void handleKeyPress(SDL_Event event);
    void move(const Uint32& timeSinceLastFrame);
};
