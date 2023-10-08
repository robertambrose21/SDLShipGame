#include "window.h"

Window::Window(int width, int height, Grid& grid) :
    width(width),
    height(height)
{
    gridRenderer = std::make_unique<GridRenderer>(grid, height);
}

Window::~Window() {
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
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

    if(TTF_Init() == -1) {
        std::cout << "SDL_ttf could not be initialised: " << TTF_GetError() << std::endl;
        return false;
    }
    
    textureLoader = TextureLoader(renderer.get());
    graphicsContext = std::make_unique<GraphicsContext>(renderer.get(), textureLoader, *gridRenderer, width, height);

    return true;
}


void Window::update(int64_t timeSinceLastFrame, bool& quit) {
    SDL_Event e;

    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT) {
            quit = true;
        }

        for(auto const& worker : eventWorkers) {
            worker(e, quit);
        }
    }

    SDL_RenderClear(renderer.get());

    gridRenderer->draw(*graphicsContext);

    for(auto const& worker : drawWorkers) {
        worker(*graphicsContext, quit);
    }

    SDL_SetRenderDrawColor(renderer.get(), 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderPresent(renderer.get());
}

void Window::addLoopDrawWorker(std::function<void(GraphicsContext&, bool&)> worker) {
    drawWorkers.push_back(worker);
}

void Window::addLoopEventWorker(std::function<void(const SDL_Event&, bool&)> worker) {
    eventWorkers.push_back(worker);
}

void Window::setGridTileTexture(int tileId, uint32_t textureId) {
    gridRenderer->setTileTexture(tileId, textureId);
}

GridRenderer& Window::getGridRenderer(void) {
    game_assert(gridRenderer != nullptr);
    return *gridRenderer;
}

TextureLoader& Window::getTextureLoader(void) {
    return textureLoader;
}

GraphicsContext& Window::getGraphicsContext(void) {
    game_assert(graphicsContext != nullptr);
    return *graphicsContext;
}
