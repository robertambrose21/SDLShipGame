#include "tile.h"

Tile::Tile(int id, Texture texture) :
    id(id),
    texture(texture)
{ }

Texture Tile::getTexture(void) {
    return texture;
}

int Tile::getId(void) const {
    return id;
}