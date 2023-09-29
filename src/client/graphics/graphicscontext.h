#pragma once

#include <SDL2/SDL.h>

#include "textureloader.h"
#include "gridrenderer.h"

class GridRenderer;

class GraphicsContext {
private:
    SDL_Renderer* renderer;
    TextureLoader& textureLoader;
    GridRenderer& gridRenderer;

public:
    GraphicsContext(
        SDL_Renderer* renderer,
        TextureLoader& textureLoader,
        GridRenderer& gridRenderer
    );

    SDL_Renderer* getRenderer(void);
    TextureLoader& getTextureLoader(void);
    GridRenderer& getGridRenderer(void);
};