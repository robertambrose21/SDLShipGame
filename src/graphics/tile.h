#pragma once

#include <SDL2/SDL.h>
#include <memory>

#include "textureloader.h"

class Tile {
private:
    int id;
    Texture texture;

public:
    Tile() { }
    Tile(int id, Texture texture);

    Texture getTexture(void);
    int getId(void) const;
};
