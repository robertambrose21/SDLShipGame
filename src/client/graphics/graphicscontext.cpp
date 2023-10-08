#include "graphicscontext.h"

GraphicsContext::GraphicsContext(
    SDL_Renderer* renderer,
    TextureLoader& textureLoader,
    GridRenderer& gridRenderer,
    int windowWidth,
    int windowHeight
) :
    renderer(renderer),
    textureLoader(textureLoader),
    gridRenderer(gridRenderer),
    windowWidth(windowWidth),
    windowHeight(windowHeight)
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

int GraphicsContext::getWindowWidth(void) const {
    return windowWidth;
}

int GraphicsContext::getWindowHeight(void) const {
    return windowHeight;
}