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

    application = std::make_unique<Application>(
        std::make_unique<Grid>(25, 25), // TODO: This should be defined by the server
        std::make_unique<EntityPool>(),
        std::make_unique<WeaponController>(),
        std::make_unique<ProjectilePool>(),
        std::make_unique<AreaOfEffectPool>(),
        std::make_unique<ClientTurnController>(),
        std::make_unique<ItemController>(),
        std::make_unique<EffectController>()
    );

    auto& context = application->getContext();

    context.getTurnController()->initialise(application->getContext());
    context.getAreaOfEffectPool()->initialise(application->getContext());
    context.getProjectilePool()->initialise(application->getContext());
    context.getWeaponController()->initialise(application->getContext());
    context.getEntityPool()->initialise(application->getContext());
    context.getItemController()->initialise(application->getContext());

    context.getTurnController()->subscribe<TurnEventData>(&stdoutSubscriber);
    context.getEntityPool()->subscribe<EntityEventData>(&stdoutSubscriber);
    context.getWeaponController()->subscribe<MeleeWeaponEventData>(&stdoutSubscriber);
    context.getProjectilePool()->subscribe<ProjectileEventData>(&stdoutSubscriber);
    context.getAreaOfEffectPool()->subscribe<AreaOfEffectEventData>(&stdoutSubscriber);
    context.getItemController()->subscribe<ItemEventData>(&stdoutSubscriber);
    context.getTurnController()->subscribe<TakeItemActionEventData>(&stdoutSubscriber);
    context.getTurnController()->subscribe<EngagementEventData>(&stdoutSubscriber);
    context.getTurnController()->subscribe<EquipItemActionEventData>(&stdoutSubscriber);

    weaponDrawStrategy = std::make_unique<WeaponDrawStrategy>();
    entityDrawStrategy = std::make_unique<EntityDrawStrategy>(weaponDrawStrategy.get());
    projectileDrawStrategy = std::make_unique<ProjectileDrawStrategy>();
    areaOfEffectDrawStrategy = std::make_unique<AreaOfEffectDrawStrategy>();
    itemDrawStrategy = std::make_unique<ItemDrawStrategy>();

    clientStateMachine = std::make_unique<ClientStateMachine>();
    clientStateMachine->setState(std::make_unique<ClientLoadingState>());

    clientMessagesReceiver = std::make_unique<GameClientMessagesReceiver>(application->getContext());

    client = std::make_unique<GameClient>(*clientMessagesReceiver, yojimbo::Address("127.0.0.1", 8081));
    clientMessagesTransmitter = std::make_unique<GameClientMessagesTransmitter>(*client);

    clientMessagesReceiver->setTransmitter(clientMessagesTransmitter.get());
    clientMessagesReceiver->subscribe<ApplyDamageEventData>(&stdoutSubscriber);

    context.getTurnController()->setOnAllParticipantsSetFunction([&]() {
        clientStateMachine->setState(std::make_unique<ClientGameLoopState>());
    });

    auto grid = context.getGrid();

    window = std::make_unique<Window>(1920, 1080, grid);
    window->initialiseWindow();

    window->setGridTileTexture(1, 4);
    window->setGridTileTexture(2, 25);
    window->setGridTileTexture(3, 26);
    window->setGridTileTexture(4, 27);
    window->setGridTileTexture(5, 28);
    window->setGridTileTexture(6, 29);
    window->setGridTileTexture(7, 30);
    window->setGridTileTexture(8, 31);
    window->setGridTileTexture(9, 32);
    window->setGridTileTexture(10, 33);
    window->setGridTileTexture(11, 34);
    window->setGridTileTexture(12, 35);
    window->setGridTileTexture(13, 36);
    window->setGridTileTexture(14, 37);
    window->setGridTileTexture(15, 38);
    
    for(auto i = 0; i < grid->getWidth(); i++) {
        for(auto j = 0; j < grid->getHeight(); j++) {
            grid->setTile(i, j, { 1, true });
        }
    }

    context.getEffectController()->subscribe<GridEffectEvent>(&window->getGraphicsContext().getGridRenderer());

    playerController = std::make_unique<PlayerController>(
        *clientMessagesTransmitter,
        application->getContext(), 
        window->getGraphicsContext()
    );
    clientMessagesReceiver->setPlayerController(playerController.get());
    clientMessagesReceiver->subscribe<ApplyDamageEventData>(playerController->getPlayerPanel());

    window->addLoopDrawWorker([&](auto graphicsContext, auto& quit) {
        draw(graphicsContext, quit);
    });
    window->addLoopEventWorker([&](auto const& e, auto& quit) {
        playerController->handleKeyPress(e);
        playerController->handleMouseEvent(e);
    });
    window->addUiWorker([&](auto graphicsContext) {
        ImGui::ShowDemoWindow();
        playerController->drawUI(graphicsContext);
    });

    application->addLogicWorker([&](ApplicationContext& c, auto const& timeSinceLastFrame, auto& quit) {
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
    auto& context = application->getContext();
    auto entityPool = context.getEntityPool();
    auto projectilePool = context.getProjectilePool();
    auto areaOfEffectPool = context.getAreaOfEffectPool();
    auto itemController = context.getItemController();

    // for(auto& [_, item] : itemController->getItems()) {
    for(auto& item : itemController->getWorldItems()) {
        itemDrawStrategy->draw(item, graphicsContext);
    }

    for(auto aoe : areaOfEffectPool->getAoeEffects()) {
        areaOfEffectDrawStrategy->draw(aoe, graphicsContext);
    }

    for(auto entity : entityPool->getEntities()) {
        entityDrawStrategy->draw(entity, graphicsContext);
    }

    for(auto projectile : projectilePool->getAllProjectiles()) {
        projectileDrawStrategy->draw(projectile, graphicsContext);
    }

    playerController->draw(graphicsContext);
}

void ClientApplication::update(int64_t timeSinceLastFrame, bool& quit) {
    auto& context = application->getContext();
    auto grid = context.getGrid();
    auto entityPool = context.getEntityPool();
    auto projectilePool = context.getProjectilePool();
    auto areaOfEffectPool = context.getAreaOfEffectPool();
    auto turnController = context.getTurnController();

    client->update(timeSinceLastFrame);
    
    switch(clientStateMachine->getCurrentState()->GetType()) {
        case ClientStateMachine::Loading:
            break;

        case ClientStateMachine::GameLoop:
            turnController->update(timeSinceLastFrame, quit);
            entityPool->updateEntities(timeSinceLastFrame, quit);
            playerController->update(timeSinceLastFrame);
            projectilePool->update(timeSinceLastFrame);
            areaOfEffectPool->update(timeSinceLastFrame);
            break;

        default:
            break;
    }
}

void ClientApplication::run(void) {
    application->run();
}