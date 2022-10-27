#include "graphicscontext.h"

GraphicsContext::GraphicsContext(
    std::shared_ptr<SDL_Renderer> renderer,
    std::shared_ptr<TextureLoader> textureLoader,
    std::shared_ptr<GridRenderer> gridRenderer
) :
    renderer(renderer),
    textureLoader(textureLoader),
    gridRenderer(gridRenderer)
{ }

std::shared_ptr<SDL_Renderer> GraphicsContext::getRenderer(void) {
    return renderer;
}

std::shared_ptr<TextureLoader> GraphicsContext::getTextureLoader(void) {
    return textureLoader;
}

std::shared_ptr<GridRenderer> GraphicsContext::getGridRenderer(void) {
    return gridRenderer;
}