#pragma once

#include <SDL2/SDL.h>

#include "../graphics/grid.h"
#include "../graphics/textureloader.h"

class PlayerController {
private:
    typedef struct {
        int x, y;
    } Position;

    std::shared_ptr<Texture> texture;
    std::shared_ptr<Grid> grid;

    Position position;
public:
    PlayerController(std::shared_ptr<Grid> grid);

    void setTexture(std::shared_ptr<Texture> texture);
    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void handleKeyPress(SDL_Event event);
};
