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

    auto context = Application::getContext();
    auto grid = context->getGrid();

    clientMessagesReceiver = std::make_shared<GameClientMessagesReceiver>(context);

    client = std::make_shared<GameClient>(clientMessagesReceiver, yojimbo::Address("127.0.0.1", 8081));
    clientMessagesTransmitter = std::make_shared<GameClientMessagesTransmitter>(client);

    Application::instance().initialise(Window::Headless::NO);

    for(auto i = 0; i < grid->getWidth(); i++) {
        for(auto j = 0; j < grid->getHeight(); j++) {
            grid->setTile(i, j, { 1, true });
        }
    }

    playerController = std::make_shared<PlayerController>(
        clientMessagesTransmitter,
        context->getWindow()->getGridRenderer(), 
        context->getEntityPool()
    );
    clientMessagesReceiver->setPlayerController(playerController);

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