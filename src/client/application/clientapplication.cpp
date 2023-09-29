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

    auto& application = Application::instance();

    auto& context = Application::getContext();
    auto& grid = context.getGrid();
    auto& entityPool = context.getEntityPool();
    auto& projectilePool = context.getProjectilePool();
    auto& areaOfEffectPool = context.getAreaOfEffectPool();
    auto& turnController = context.getTurnController();

    weaponDrawStrategy = std::make_unique<WeaponDrawStrategy>();
    entityDrawStrategy = std::make_unique<EntityDrawStrategy>(weaponDrawStrategy.get());
    projectileDrawStrategy = std::make_unique<ProjectileDrawStrategy>();
    areaOfEffectDrawStrategy = std::make_unique<AreaOfEffectDrawStrategy>();

    clientStateMachine = std::make_unique<ClientStateMachine>();
    clientStateMachine->setState(std::make_unique<ClientLoadingState>());

    clientMessagesReceiver = std::make_unique<GameClientMessagesReceiver>(context);

    client = std::make_unique<GameClient>(*clientMessagesReceiver, yojimbo::Address("127.0.0.1", 8081));
    clientMessagesTransmitter = std::make_unique<GameClientMessagesTransmitter>(*client);

    clientMessagesReceiver->setTransmitter(clientMessagesTransmitter.get());

    turnController.setOnAllParticipantsSetFunction([&]() {
        clientStateMachine->setState(std::make_unique<ClientGameLoopState>());
    });

    application.initialise();

    window = std::make_unique<Window>(1920, 1080, grid);
    window->initialiseWindow();

    window->setGridTileTexture(1, 4);
    window->setGridTileTexture(2, 5);
    
    for(auto i = 0; i < grid.getWidth(); i++) {
        for(auto j = 0; j < grid.getHeight(); j++) {
            grid.setTile(i, j, { 1, true });
        }
    }

    playerController = std::make_unique<PlayerController>(
        *clientMessagesTransmitter, context, window->getGraphicsContext()
    );
    clientMessagesReceiver->setPlayerController(playerController.get());

    window->addLoopDrawWorker([&](auto graphicsContext, auto& quit) {
        draw(graphicsContext, quit);
    });
    window->addLoopEventWorker([&](auto const& e, auto& quit) {
        playerController->handleKeyPress(e);
        playerController->handleMouseEvent(e);
    });

    application.addLogicWorker([&](auto const& timeSinceLastFrame, auto& quit) {
        update(timeSinceLastFrame, quit);
        window->update(timeSinceLastFrame, quit);
    });
}

void ClientApplication::draw(GraphicsContext& graphicsContext, bool& quit) {
    switch(clientStateMachine->getCurrentState()->GetType()) {
        case ClientStateMachine::Loading:
            break;

        case ClientStateMachine::GameLoop:
            drawGameLoop(graphicsContext);
            break;

        default:
            break;
    }
}

void ClientApplication::drawGameLoop(GraphicsContext& graphicsContext) {
    auto& context = Application::getContext();
    auto& entityPool = context.getEntityPool();
    auto& projectilePool = context.getProjectilePool();
    auto& areaOfEffectPool = context.getAreaOfEffectPool();

    for(auto aoe : areaOfEffectPool.getAoeEffects()) {
        areaOfEffectDrawStrategy->draw(aoe, graphicsContext);
    }

    for(auto entity : entityPool.getEntities()) {
        entityDrawStrategy->draw(entity, graphicsContext);
    }

    for(auto projectile : projectilePool.getAllProjectiles()) {
        projectileDrawStrategy->draw(projectile, graphicsContext);
    }

    playerController->draw(graphicsContext);
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