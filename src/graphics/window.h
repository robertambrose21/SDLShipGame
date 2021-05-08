#pragma once

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <memory>
#include <iostream>
#include <functional>

#include "textureloader.h"
#include "gridrenderer.h"

// TODO: Modules (eventually): https://docs.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-160
class Window {
private:
    struct sdl_deleter {
        void operator()(SDL_Window *p) const { }
        void operator()(SDL_Renderer *p) const { }
    };

    int width;
    int height;

    std::shared_ptr<SDL_Window> window;
    std::shared_ptr<SDL_Renderer> renderer;

    std::shared_ptr<TextureLoader> textureLoader;
    std::shared_ptr<GridRenderer> grid;

    std::vector<std::function<void(const Uint32&, bool&)>> logicWorkers;
    std::vector<std::function<void(std::shared_ptr<SDL_Renderer>, bool&)>> drawWorkers;
    std::vector<std::function<void(SDL_Event, bool&)>> eventWorkers;
    
public:
    Window(int width, int height);
    ~Window();
 
    bool initialiseWindow(void);
    void loop(void);

    void addLoopLogicWorker(std::function<void(const Uint32&, bool&)> worker);
    void addLoopDrawWorker(std::function<void(std::shared_ptr<SDL_Renderer>, bool&)> worker);
    void addLoopEventWorker(std::function<void(SDL_Event, bool&)> worker);
    
    void setGridTileTexture(int x, int y, const std::string& texture);
    std::shared_ptr<GridRenderer> getGrid(void);

    // TODO: Think about a better place to put this
    std::shared_ptr<TextureLoader> getTextureLoader(void);
};
