#pragma once

#include <SDL2/SDL.h>

#include "textureloader.h"
#include "gridrenderer.h"

class GridRenderer;

class GraphicsContext {
private:
    std::shared_ptr<SDL_Renderer> renderer;
    std::shared_ptr<TextureLoader> textureLoader;
    std::shared_ptr<GridRenderer> gridRenderer;

public:
    GraphicsContext(
        std::shared_ptr<SDL_Renderer> renderer,
        std::shared_ptr<TextureLoader> textureLoader,
        std::shared_ptr<GridRenderer> gridRenderer
    );

    std::shared_ptr<SDL_Renderer> getRenderer(void);
    std::shared_ptr<TextureLoader> getTextureLoader(void);
    std::shared_ptr<GridRenderer> getGridRenderer(void);
};