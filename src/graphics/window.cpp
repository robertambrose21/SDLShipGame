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

    grid = std::make_shared<GridRenderer>(20, 20, height);

    return true;
}

void Window::loop(void) {
    bool quit = false;
    SDL_Event e;

    auto startTime = SDL_GetTicks();
    auto currentTime = startTime;
    auto timeSinceLastFrame = 0;

    while(!quit) {
        timeSinceLastFrame = SDL_GetTicks() - currentTime;
        currentTime = SDL_GetTicks();

        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }

            for(auto worker : eventWorkers) {
                worker(e, quit);
            }
        }

        for(auto worker : logicWorkers) {
            worker(timeSinceLastFrame, quit);
        }

        SDL_RenderClear(renderer.get());

        grid->draw(renderer);

        for(auto worker : drawWorkers) {
            worker(renderer, quit);
        }

        SDL_RenderPresent(renderer.get());
    }
}

void Window::addLoopLogicWorker(std::function<void(const Uint32&, bool&)> worker) {
    logicWorkers.push_back(worker);
}

void Window::addLoopDrawWorker(std::function<void(std::shared_ptr<SDL_Renderer>, bool&)> worker) {
    drawWorkers.push_back(worker);
}

void Window::addLoopEventWorker(std::function<void(SDL_Event, bool&)> worker) {
    eventWorkers.push_back(worker);
}

void Window::setGridTileTexture(int x, int y, const std::string& texture) {
    grid->setTile(x, y, textureLoader->getTexture(texture));
}

std::shared_ptr<GridRenderer> Window::getGrid(void) {
    return grid;
}

std::shared_ptr<TextureLoader> Window::getTextureLoader(void) {
    return textureLoader;
}
