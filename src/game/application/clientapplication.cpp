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
    clientMessagesController = std::make_shared<ClientMessagesController>(client);

    Application::instance().initialise(Window::Headless::NO);

    auto context = Application::instance().getContext();

    playerController = std::make_shared<PlayerController>(
        clientMessagesController,
        context->getWindow()->getGridRenderer(), 
        context->getEntityPool()
    );
    playerController->setParticipant(context->getTurnController()->getParticipant(0));

    context->getWindow()->addLoopLogicWorker([&](auto timeSinceLastFrame, auto& quit) {
        client->update(timeSinceLastFrame);
    });

    context->getWindow()->addLoopEventWorker([&](auto e, auto& quit) {
        playerController->handleKeyPress(e);
        playerController->handleMouseEvent(e);
    });
}

void ClientApplication::run(void) {
    Application::instance().run();
}