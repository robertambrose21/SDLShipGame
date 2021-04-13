#include "window.h"

Window::Window(int width, int height) :
    width(width),
    height(height)
{ }

Window::~Window() {
    IMG_Quit();
}

bool Window::initialiseWindow(void) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not be initialized: " << SDL_GetError() << std::endl;
        return false;
    }

    window = std::unique_ptr<SDL_Window, sdl_deleter>(
        SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN),
        sdl_deleter()
    );

    if(window == nullptr) {
        std::cout << "SDL Window could not be created: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = std::unique_ptr<SDL_Renderer, sdl_deleter>(
        SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED),
        sdl_deleter()
    );

    if(renderer == nullptr) {
        std::cout << "SDL Renderer could not be created: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetRenderDrawColor(renderer.get(), 0x00, 0x00, 0x00, 0xFF);

    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "SDL_image could not be initialised: " << IMG_GetError() << std::endl;
        return false;
    }

    textureLoader = std::make_shared<TextureLoader>(renderer);

    grid = std::make_shared<Grid>(20, 20, height);
    for(auto i = 0; i < 20; i++) {
        for(auto j = 0; j < 20; j++) {
            grid->setTile(i, j, Tile(1, textureLoader->getTexture("../assets/floor1.png")));
        }
    }

    return true;
}

void Window::loop(void) {
    bool quit = false;
    SDL_Event e;

    while(!quit) {
        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_RenderClear(renderer.get());
        grid->draw(renderer);
        SDL_RenderPresent(renderer.get());
    }
}

std::shared_ptr<Grid> Window::getGrid(void) {
    return grid;
}

void Window::setGrid(std::shared_ptr<Grid> grid) {
    this->grid = grid;
}