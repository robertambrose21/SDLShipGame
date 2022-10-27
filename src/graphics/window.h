#pragma once

// TODO: Might need to fix up these imports for non-linux dev
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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

    std::vector<std::function<void(const Uint32&, bool&)>> logicWorkers;
    std::vector<std::function<void(std::shared_ptr<GraphicsContext>, bool&)>> drawWorkers;
    std::vector<std::function<void(SDL_Event, bool&)>> eventWorkers;
    
public:
    Window(int width, int height, std::shared_ptr<Grid> grid);
    ~Window();
 
    bool initialiseWindow(bool headless);
    void loop(void);

    void addLoopLogicWorker(std::function<void(const Uint32&, bool&)> worker);
    void addLoopDrawWorker(std::function<void(std::shared_ptr<GraphicsContext>, bool&)> worker);
    void addLoopEventWorker(std::function<void(SDL_Event, bool&)> worker);
    
    void setGridTileTexture(const int& tileId, const std::string& texture);

    std::shared_ptr<GridRenderer> getGridRenderer(void);
    std::shared_ptr<TextureLoader> getTextureLoader(void);

    std::shared_ptr<GraphicsContext> getGraphicsContext(void);
};
