#include "clientapplication.h"

ClientApplication::ClientApplication()
{ }

ClientApplication::~ClientApplication() {
    ShutdownYojimbo();
}

void ClientApplication::initialise(void) {
    if (!InitializeYojimbo()) {
        std::cout << "error: failed to initialize Yojimbo!\n" << std::endl;
        return;
    }

    client = std::make_shared<GameClient>(yojimbo::Address("127.0.0.1", 8081));

    Application::instance().initialise();

    auto window = Application::instance().getContext()->getWindow();

    window->addLoopLogicWorker([&](auto timeSinceLastFrame, auto& quit) {
        client->update(timeSinceLastFrame);
    });

    window->addLoopEventWorker([&](auto e, auto& quit) {
        if(e.type == SDL_KEYDOWN) {
            switch(e.key.keysym.sym) {
                case SDLK_k:
                    client->sendMessage();
                    break;

                default:
                    break;
            }
        }
    });
}

void ClientApplication::run(void) {
    Application::instance().run();
}