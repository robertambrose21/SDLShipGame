#pragma once

#include "sdlimport.h"
#include <memory>
#include <iostream>
#include <functional>

#include "textureloader.h"
#include "gridrenderer.h"
#include "graphicscontext.h"

// TODO: Modules (eventually): https://docs.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-160
class Window {
private:
    struct sdl_deleter {
        void operator()(SDL_Window *p) const { }
        void operator()(SDL_Renderer *p) const { }
    };

    bool headless;

    int width;
    int height;

    std::shared_ptr<SDL_Window> window;

    std::shared_ptr<GraphicsContext> graphicsContext;
    std::shared_ptr<SDL_Renderer> renderer;
    std::shared_ptr<TextureLoader> textureLoader;
    std::shared_ptr<GridRenderer> gridRenderer;

    std::vector<std::function<void(uint32_t, bool&)>> logicWorkers;
    std::vector<std::function<void(const std::shared_ptr<GraphicsContext>&, bool&)>> drawWorkers;
    std::vector<std::function<void(const SDL_Event&, bool&)>> eventWorkers;
    
public:
    enum Headless {
        YES,
        NO
    };

    Window(int width, int height, const std::shared_ptr<Grid>& grid);
    ~Window();
 
    bool initialiseWindow(const Headless& headless);
    void loop(void);

    void addLoopLogicWorker(std::function<void(uint32_t, bool&)> worker);
    void addLoopDrawWorker(std::function<void(const std::shared_ptr<GraphicsContext>&, bool&)> worker);
    void addLoopEventWorker(std::function<void(const SDL_Event&, bool&)> worker);
    
    void setGridTileTexture(int tileId, uint32_t textureId);

    std::shared_ptr<GridRenderer> getGridRenderer(void);
    std::shared_ptr<TextureLoader> getTextureLoader(void);

    std::shared_ptr<GraphicsContext> getGraphicsContext(void);
};
