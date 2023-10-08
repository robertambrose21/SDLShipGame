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
    int windowWidth;
    int windowHeight;

public:
    GraphicsContext(
        SDL_Renderer* renderer,
        TextureLoader& textureLoader,
        GridRenderer& gridRenderer,
        int windowWidth,
        int windowHeight
    );

    SDL_Renderer* getRenderer(void);
    TextureLoader& getTextureLoader(void);
    GridRenderer& getGridRenderer(void);
    int getWindowWidth(void) const;
    int getWindowHeight(void) const;
};