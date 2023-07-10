#include "window.h"

Window::Window(int width, int height, const std::shared_ptr<Grid>& grid) :
    width(width),
    height(height)
{
    gridRenderer = std::make_shared<GridRenderer>(grid, height);
}

Window::~Window() {
    IMG_Quit();
}

bool Window::initialiseWindow(const Headless& headless) {
    this->headless = headless;

    if(headless == Headless::YES) {
        return true;
    }

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
    graphicsContext = std::make_shared<GraphicsContext>(renderer, textureLoader, gridRenderer);

    return true;
}

// TODO: Consider moving loop logic out of the graphics module
void Window::loop(void) {
    bool quit = false;
    SDL_Event e;

    uint32_t currentTime = SDL_GetTicks();
    uint32_t timeSinceLastFrame = 0;

    while(!quit) {
        timeSinceLastFrame = SDL_GetTicks() - currentTime;
        currentTime = SDL_GetTicks();

        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }

            for(auto const& worker : eventWorkers) {
                worker(e, quit);
            }
        }

        for(auto const& worker : logicWorkers) {
            worker(timeSinceLastFrame, quit);
        }

        if(headless == Headless::NO) {
            SDL_RenderClear(renderer.get());

            gridRenderer->draw(graphicsContext);

            for(auto const& worker : drawWorkers) {
                worker(graphicsContext, quit);
            }

            SDL_SetRenderDrawColor(renderer.get(), 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderPresent(renderer.get());
        }
    }
}

void Window::addLoopLogicWorker(std::function<void(uint32_t, bool&)> worker) {
    logicWorkers.push_back(worker);
}

void Window::addLoopDrawWorker(std::function<void(const std::shared_ptr<GraphicsContext>&, bool&)> worker) {
    drawWorkers.push_back(worker);
}

void Window::addLoopEventWorker(std::function<void(const SDL_Event&, bool&)> worker) {
    eventWorkers.push_back(worker);
}

void Window::setGridTileTexture(int tileId, uint32_t textureId) {
    gridRenderer->setTileTexture(tileId, textureId);
}

std::shared_ptr<GridRenderer> Window::getGridRenderer(void) {
    return gridRenderer;
}

std::shared_ptr<TextureLoader> Window::getTextureLoader(void) {
    return textureLoader;
}

std::shared_ptr<GraphicsContext> Window::getGraphicsContext(void) {
    game_assert(graphicsContext != nullptr);
    return graphicsContext;
}
