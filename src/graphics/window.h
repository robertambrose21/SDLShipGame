#pragma once

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <memory>
#include <iostream>

#include "textureloader.h"
#include "grid.h"

// TODO: Modules: https://docs.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-160
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
    std::shared_ptr<Grid> grid;
    
public:
    Window(int width, int height);
    ~Window();

    bool initialiseWindow(void);
    void loop(void);
    
    void setGrid(std::shared_ptr<Grid> grid);
    std::shared_ptr<Grid> getGrid(void);
};
