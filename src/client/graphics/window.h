#pragma once

#include "sdlimport.h"
#include <memory>
#include <iostream>
#include <functional>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

#include "textureloader.h"
#include "gridrenderer.h"
#include "graphicscontext.h"

// TODO: Modules (eventually): https://docs.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-160
class Window {
private:
    struct sdl_deleter {
        void operator()(SDL_Window *p) const { SDL_DestroyWindow(p); }
        void operator()(SDL_Renderer *p) const { SDL_DestroyRenderer(p); }
    };

    int width;
    int height;

    std::unique_ptr<SDL_Window, sdl_deleter> window;

    std::unique_ptr<GraphicsContext> graphicsContext;
    std::unique_ptr<SDL_Renderer, sdl_deleter> renderer;
    TextureLoader textureLoader;
    std::unique_ptr<GridRenderer> gridRenderer;

    std::vector<std::function<void(GraphicsContext&, bool&)>> drawWorkers;
    std::vector<std::function<void(const SDL_Event&, bool&)>> eventWorkers;
    std::vector<std::function<void(GraphicsContext&)>> uiWorkers;

    void initialiseImgui(void);
    
public:
    Window(
        int width, 
        int height, 
        Grid* grid, 
        VisiblityController* visiblityController, 
        EntityPool* entityPool
    );
    ~Window();
 
    bool initialiseWindow(void);
    void update(int64_t timeSinceLastFrame, bool& quit);

    void addLoopDrawWorker(std::function<void(GraphicsContext&, bool&)> worker);
    void addLoopEventWorker(std::function<void(const SDL_Event&, bool&)> worker);
    void addUiWorker(std::function<void(GraphicsContext&)> worker);
    
    void setGridTileTexture(int tileId, uint32_t textureId);

    GridRenderer& getGridRenderer(void);
    TextureLoader& getTextureLoader(void);
    GraphicsContext& getGraphicsContext(void);
};
