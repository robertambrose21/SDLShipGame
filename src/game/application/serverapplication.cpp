#include "serverapplication.h"

ServerApplication::ServerApplication()
{ }

ServerApplication::~ServerApplication() {
    ShutdownYojimbo();
}

void ServerApplication::initialise(void) {
    if (!InitializeYojimbo()) {
        std::cout << "error: failed to initialize Yojimbo!\n" << std::endl;
        return;
    }

    Application::instance().initialise(Window::Headless::YES);

    receiver = std::make_shared<GameServerMessagesReceiver>();
    server = std::make_shared<GameServer>(receiver, yojimbo::Address("127.0.0.1", 8081));

    Application::instance().getContext()->getWindow()->addLoopLogicWorker([&](auto timeSinceLastFrame, auto& quit) {
        server->update(timeSinceLastFrame);
    });
}

void ServerApplication::run(void) {
    Application::instance().run();
}