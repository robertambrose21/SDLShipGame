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

    clientMessagesReceiver = std::make_shared<GameClientMessagesReceiver>();

    client = std::make_shared<GameClient>(clientMessagesReceiver, yojimbo::Address("127.0.0.1", 8081));
    clientMessagesTransmitter = std::make_shared<GameClientMessagesTransmitter>(client);

    Application::instance().initialise(Window::Headless::NO);

    auto context = Application::instance().getContext();

    playerController = std::make_shared<PlayerController>(
        clientMessagesTransmitter,
        context->getWindow()->getGridRenderer(), 
        context->getEntityPool()
    );
    // Receive participant id from server when we connect - then can ensure when we pass etc it's from a valid participant
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