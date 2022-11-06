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

    auto context = Application::instance().getContext();

    receiver = std::make_shared<GameServerMessagesReceiver>(context);
    server = std::make_shared<GameServer>(receiver, yojimbo::Address("127.0.0.1", 8081));
    transmitter = std::make_shared<GameServerMessagesTransmitter>(server);

    context->getWindow()->addLoopLogicWorker([&](auto timeSinceLastFrame, auto& quit) {
        server->update(timeSinceLastFrame);
    });

    context->getTurnController()->addOnNextTurnFunction([&](int currentParticipant, int turnNumber) {
        transmitter->sendGameStateUpdate(context->getCurrentGameState());
    });
}

void ServerApplication::run(void) {
    Application::instance().run();
}