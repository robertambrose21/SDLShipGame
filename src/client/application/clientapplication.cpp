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

    auto& context = Application::getContext();
    auto& grid = context.getGrid();
    auto& entityPool = context.getEntityPool();
    auto& projectilePool = context.getProjectilePool();
    auto& areaOfEffectPool = context.getAreaOfEffectPool();
    auto& turnController = context.getTurnController();

    clientStateMachine = std::make_unique<ClientStateMachine>();
    clientStateMachine->setState(std::make_unique<ClientLoadingState>());

    clientMessagesReceiver = std::make_unique<GameClientMessagesReceiver>(context);

    client = std::make_unique<GameClient>(*clientMessagesReceiver, yojimbo::Address("127.0.0.1", 8081));
    clientMessagesTransmitter = std::make_unique<GameClientMessagesTransmitter>(*client);

    clientMessagesReceiver->setTransmitter(clientMessagesTransmitter.get());

    turnController.setOnAllParticipantsSetFunction([&]() {
        clientStateMachine->setState(std::make_unique<ClientGameLoopState>());
    });

    Application::instance().initialise(Window::Headless::NO);

    auto& window = context.getWindow();
    
    for(auto i = 0; i < grid.getWidth(); i++) {
        for(auto j = 0; j < grid.getHeight(); j++) {
            grid.setTile(i, j, { 1, true });
        }
    }

    playerController = std::make_unique<PlayerController>(
        *clientMessagesTransmitter, context
    );
    clientMessagesReceiver->setPlayerController(playerController.get());

    window.addLoopLogicWorker([&](auto const& timeSinceLastFrame, auto& quit) {
        update(timeSinceLastFrame, quit);
    });
    window.addLoopDrawWorker([&](auto graphicsContext, auto& quit) {
        draw(graphicsContext, quit);
    });
    window.addLoopEventWorker([&](auto const& e, auto& quit) {
        playerController->handleKeyPress(e);
        playerController->handleMouseEvent(e);
    });
}

void ClientApplication::draw(GraphicsContext& graphicsContext, bool& quit) {
    auto& context = Application::getContext();
    auto& entityPool = context.getEntityPool();
    auto& projectilePool = context.getProjectilePool();
    auto& areaOfEffectPool = context.getAreaOfEffectPool();

    switch(clientStateMachine->getCurrentState()->GetType()) {
        case ClientStateMachine::Loading:
            break;

        case ClientStateMachine::GameLoop:
            projectilePool.draw(graphicsContext);
            areaOfEffectPool.draw(graphicsContext);
            entityPool.drawEntities(graphicsContext);
            playerController->draw(graphicsContext);
            break;

        default:
            break;
    }
}

void ClientApplication::update(uint32_t timeSinceLastFrame, bool& quit) {
    auto& context = Application::getContext();
    auto& grid = context.getGrid();
    auto& entityPool = context.getEntityPool();
    auto& projectilePool = context.getProjectilePool();
    auto& areaOfEffectPool = context.getAreaOfEffectPool();
    auto& turnController = context.getTurnController();

    client->update(timeSinceLastFrame);
    
    switch(clientStateMachine->getCurrentState()->GetType()) {
        case ClientStateMachine::Loading:
            break;

        case ClientStateMachine::GameLoop:
            turnController.update(timeSinceLastFrame, quit);
            entityPool.updateEntities(timeSinceLastFrame, quit);
            playerController->update(timeSinceLastFrame);
            projectilePool.update(timeSinceLastFrame);
            areaOfEffectPool.update(timeSinceLastFrame);
            break;

        default:
            break;
    }
}

void ClientApplication::run(void) {
    Application::instance().run();
}