#include "clientapplication.h"

ClientApplication::ClientApplication()
{ }

ClientApplication::~ClientApplication() {
    client.reset(); // Free client resources and disconnect so we can cleanly shutdown yojimbo
    ShutdownYojimbo();
}

void ClientApplication::initialise(void) {
    if (!InitializeYojimbo()) {
        std::cout << "error: failed to initialize Yojimbo!\n" << std::endl;
        return;
    }

    #if !defined(NDEBUG)
        spdlog::set_level(spdlog::level::trace);
        spdlog::debug("Debug logging enabled");
    #else
        spdlog::set_level(spdlog::level::info);
    #endif

    application = std::make_unique<Application>(
        std::make_unique<Grid>(128, 128), // TODO: This should be defined by the server
        std::make_unique<EntityPool>(),
        std::make_unique<WeaponController>(),
        std::make_unique<ProjectilePool>(),
        std::make_unique<AreaOfEffectPool>(),
        std::make_unique<ClientGameController>(),
        std::make_unique<ItemController>(),
        std::make_unique<EffectController>(),
        std::make_unique<SpawnController>(),
        std::make_unique<VisiblityController>()
    );

    auto& context = application->getContext();

    context.getGameController()->initialise(application->getContext());
    context.getAreaOfEffectPool()->initialise(application->getContext());
    context.getProjectilePool()->initialise(application->getContext());
    context.getWeaponController()->initialise(application->getContext());
    context.getEntityPool()->initialise(application->getContext());
    context.getItemController()->initialise(application->getContext());
    context.getSpawnController()->initialise(application->getContext());
    context.getVisibilityController()->initialise(application->getContext());
    context.getEffectController()->initialise(application->getContext());
    context.getEntityPool()->subscribe<EntityEventData>(&stdoutSubscriber);
    context.getWeaponController()->subscribe<MeleeWeaponEventData>(&stdoutSubscriber);
    context.getProjectilePool()->subscribe<ProjectileEventData>(&stdoutSubscriber);
    context.getAreaOfEffectPool()->subscribe<AreaOfEffectEventData>(&stdoutSubscriber);
    context.getItemController()->subscribe<ItemEventData>(&stdoutSubscriber);
    context.getGameController()->subscribe<TakeItemActionEventData>(&stdoutSubscriber);
    context.getGameController()->subscribe<EquipItemActionEventData>(&stdoutSubscriber);
    // TODO: Gross af - fix these subscriptions
    context.getGameController()->getEngagementController()
        ->subscribe<RemoveEngagementEventData>(dynamic_cast<ClientGameController*>(context.getGameController()));

    weaponDrawStrategy = std::make_unique<WeaponDrawStrategy>();
    entityDrawStrategy = std::make_unique<EntityDrawStrategy>(weaponDrawStrategy.get());
    projectileDrawStrategy = std::make_unique<ProjectileDrawStrategy>();
    areaOfEffectDrawStrategy = std::make_unique<AreaOfEffectDrawStrategy>();
    itemDrawStrategy = std::make_unique<ItemDrawStrategy>();

    // TODO: The tileset to use should come from the server
    auto tileSet = WFCTileSet("../assets/data/tilesets/grass_and_rocks/rules.json");
    tileSet.load();

    clientStateMachine = std::make_unique<ClientStateMachine>();
    clientStateMachine->setState(std::make_unique<ClientLoadingState>());

    // TODO: Generate this from some kind of web server
    uint64_t clientId = randomRange(1000, 9999);

    clientMessagesReceiver = std::make_unique<GameClientMessagesReceiver>(
        application->getContext(),
        clientId,
        tileSet.getWalkableTiles()
    );

    client = std::make_unique<GameClient>(
        std::make_unique<GameMessageLogger>("client_messages.log"),
        clientId,
        *clientMessagesReceiver,
        yojimbo::Address("127.0.0.1", 8081)
    );
    clientMessagesTransmitter = std::make_unique<GameClientMessagesTransmitter>(*client);

    clientMessagesReceiver->setTransmitter(clientMessagesTransmitter.get());
    clientMessagesReceiver->subscribe<ApplyDamageEventData>(&stdoutSubscriber);

    context.getGameController()->setOnAllParticipantsSetFunction([&]() {
        clientStateMachine->setState(std::make_unique<ClientGameLoopState>());
    });

    auto grid = context.getGrid();

    window = std::make_unique<Window>(
        1920, 
        1080, 
        grid, 
        context.getVisibilityController(),
        context.getEntityPool()
    );
    window->initialiseWindow();

    for(auto const& [_, tile] : tileSet.getTileMapping()) {
        window->setGridTileTexture(tile.id, tile.textureId);
    }
    
    for(auto i = 0; i < grid->getWidth(); i++) {
        for(auto j = 0; j < grid->getHeight(); j++) {
            grid->setTile(i, j, { 1, false, false });
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

    for(auto& item : itemController->getWorldItems()) {
        itemDrawStrategy->draw(item, graphicsContext);
    }


    for(auto const& [_, aoes] : areaOfEffectPool->getEngagementAoEs()) {
        for(auto const& aoe : aoes) {
            areaOfEffectDrawStrategy->draw(aoe.get(), graphicsContext);
        }
    }
    
    for(auto const& aoe : areaOfEffectPool->getAdhocAoEs()) {
        areaOfEffectDrawStrategy->draw(aoe.get(), graphicsContext);
    }

    for(auto entity : entityPool->getEntities()) {
        entityDrawStrategy->draw(entity, graphicsContext);
    }

    for(auto projectile : projectilePool->getAllProjectiles()) {
        projectileDrawStrategy->draw(projectile, graphicsContext);
    }

    playerController->draw(graphicsContext);
}

void ClientApplication::selectEntityOnStartupHack(void) {
    static bool isSelected = false;

    if(isSelected) {
        return;
    }

    if(
        playerController->getParticipant() != nullptr && 
        !playerController->getParticipant()->getEntities().empty()
    ) {
        playerController->selectAll();
        isSelected = true;
    }
}

void ClientApplication::update(int64_t timeSinceLastFrame, bool& quit) {
    auto& context = application->getContext();
    auto grid = context.getGrid();
    auto entityPool = context.getEntityPool();
    auto projectilePool = context.getProjectilePool();
    auto areaOfEffectPool = context.getAreaOfEffectPool();
    auto gameController = context.getGameController();
    auto effectController = context.getEffectController();

    client->update(timeSinceLastFrame);
    selectEntityOnStartupHack();
    
    switch(clientStateMachine->getCurrentState()->GetType()) {
        case ClientStateMachine::Loading:
            break;

        case ClientStateMachine::GameLoop:
            gameController->update(timeSinceLastFrame, quit);
            entityPool->updateEntities(timeSinceLastFrame, quit);
            playerController->update(timeSinceLastFrame);
            projectilePool->update(timeSinceLastFrame);
            areaOfEffectPool->update(timeSinceLastFrame);
            effectController->update(timeSinceLastFrame);
            break;

        default:
            break;
    }
}

void ClientApplication::run(void) {
    application->run();
}