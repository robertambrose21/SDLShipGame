#include "graphicscontext.h"

GraphicsContext::GraphicsContext(
    SDL_Renderer* renderer,
    TextureLoader& textureLoader,
    GridRenderer& gridRenderer
) :
    renderer(renderer),
    textureLoader(textureLoader),
    gridRenderer(gridRenderer)
{
    game_assert(renderer != nullptr);
}

SDL_Renderer* GraphicsContext::getRenderer(void) {
    return renderer;
}

TextureLoader& GraphicsContext::getTextureLoader(void) {
    return textureLoader;
}

GridRenderer& GraphicsContext::getGridRenderer(void) {
    return gridRenderer;
}