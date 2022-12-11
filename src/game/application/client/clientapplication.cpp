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
    entityPool = context->getEntityPool();
    projectilePool = context->getProjectilePool();
    areaOfEffectPool = context->getAreaOfEffectPool();
    turnController = context->getTurnController();

    clientStateMachine = std::make_shared<ClientStateMachine>();
    clientStateMachine->setState(std::make_shared<ClientLoadingState>());

    clientMessagesReceiver = std::make_shared<GameClientMessagesReceiver>(context);

    client = std::make_shared<GameClient>(clientMessagesReceiver, yojimbo::Address("127.0.0.1", 8081));
    clientMessagesTransmitter = std::make_shared<GameClientMessagesTransmitter>(client);

    clientMessagesReceiver->setTransmitter(clientMessagesTransmitter);

    turnController->setOnAllParticipantsSetFunction([&]() {
        clientStateMachine->setState(std::make_shared<ClientGameLoopState>());
    });

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
        update(timeSinceLastFrame, quit);
    });
    context->getWindow()->addLoopDrawWorker([&](auto graphicsContext, auto& quit) {
        draw(graphicsContext, quit);
    });
    context->getWindow()->addLoopEventWorker([&](auto e, auto& quit) {
        playerController->handleKeyPress(e);
        playerController->handleMouseEvent(e);
    });
}

void ClientApplication::draw(std::shared_ptr<GraphicsContext> graphicsContext, bool& quit) {
    switch(clientStateMachine->getCurrentState()->GetType()) {
        case ClientStateMachine::Loading:
            break;

        case ClientStateMachine::GameLoop:
            entityPool->drawEntities(graphicsContext);
            projectilePool->draw(graphicsContext);
            areaOfEffectPool->draw(graphicsContext);
            break;

        default:
            break;
    }
}

void ClientApplication::update(uint32_t timeSinceLastFrame, bool& quit) {
    client->update(timeSinceLastFrame);
    
    switch(clientStateMachine->getCurrentState()->GetType()) {
        case ClientStateMachine::Loading:
            break;

        case ClientStateMachine::GameLoop:
            turnController->update(timeSinceLastFrame);
            entityPool->updateEntities(timeSinceLastFrame, quit);
            projectilePool->update(timeSinceLastFrame);
            areaOfEffectPool->update(timeSinceLastFrame);
            break;

        default:
            break;
    }
}

void ClientApplication::run(void) {
    Application::instance().run();
}