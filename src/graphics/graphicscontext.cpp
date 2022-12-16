#include "graphicscontext.h"

GraphicsContext::GraphicsContext(
    const std::shared_ptr<SDL_Renderer>& renderer,
    const std::shared_ptr<TextureLoader>& textureLoader,
    const std::shared_ptr<GridRenderer>& gridRenderer
) :
    renderer(renderer),
    textureLoader(textureLoader),
    gridRenderer(gridRenderer)
{
    game_assert(renderer != nullptr);
    game_assert(textureLoader != nullptr);
    game_assert(gridRenderer != nullptr);
}

std::shared_ptr<SDL_Renderer> GraphicsContext::getRenderer(void) {
    return renderer;
}

std::shared_ptr<TextureLoader> GraphicsContext::getTextureLoader(void) {
    return textureLoader;
}

std::shared_ptr<GridRenderer> GraphicsContext::getGridRenderer(void) {
    return gridRenderer;
}