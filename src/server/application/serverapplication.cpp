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

    application = std::make_unique<Application>(
        std::make_unique<Grid>(100, 100),
        std::make_unique<EntityPool>(),
        std::make_unique<WeaponController>(),
        std::make_unique<ProjectilePool>(),
        std::make_unique<AreaOfEffectPool>(),
        std::make_unique<ServerTurnController>(),
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

    server = std::make_unique<GameServer>(yojimbo::Address("127.0.0.1", 8081));

    receiver = std::make_unique<GameServerMessagesReceiver>(application->getContext());
    transmitter = std::make_unique<GameServerMessagesTransmitter>(
        *server, 
        dynamic_cast<ServerTurnController*>(context.getTurnController()),
        [&](int clientIndex) {
            onClientConnect(clientIndex);
        }
    );
    receiver->setTransmitter(transmitter.get());

    dynamic_cast<ServerTurnController*>(context.getTurnController())->setTransmitter(transmitter.get());

    server->setReceiver(receiver.get());
    server->setTransmitter(transmitter.get());
    context.setServerMessagesTransmitter(transmitter.get());
    context.getItemController()->subscribe<ItemEventData>(transmitter.get());
    context.getEntityPool()->subscribe(context.getItemController());
    context.getTurnController()->subscribe<MoveActionEventData>(transmitter.get());
    context.getTurnController()->subscribe<AttackActionEventData>(transmitter.get());
    context.getTurnController()->subscribe<TakeItemActionEventData>(transmitter.get());
    context.getTurnController()->subscribe<EngagementEventData>(transmitter.get());
    context.getAreaOfEffectPool()->subscribe<AreaOfEffectEventData>(transmitter.get());
    context.getProjectilePool()->subscribe<ProjectileEventData>(transmitter.get());
    context.getWeaponController()->subscribe<MeleeWeaponEventData>(transmitter.get());
    context.getEffectController()->subscribe<EntityEffectEvent>(transmitter.get());
    context.getEffectController()->subscribe<GridEffectEvent>(transmitter.get());

    application->addLogicWorker([&](ApplicationContext& c, auto const& timeSinceLastFrame, auto& quit) {
        server->update(timeSinceLastFrame);
        c.getTurnController()->update(timeSinceLastFrame, quit);
        c.getEntityPool()->updateEntities(timeSinceLastFrame, quit);
        c.getProjectilePool()->update(timeSinceLastFrame);
        c.getAreaOfEffectPool()->update(timeSinceLastFrame);
    });

    // TODO: This somehow makes the game state messages get received first rather than the set participant ones.
    // On the client side we need to wait until participants are all set before we load the rest of the map
    context.getTurnController()->addOnNextTurnFunction([&](auto const& currentParticipantId, auto const& turnNumber) {
        // TODO: This has a synching issue currently particularly around entity movement not completing in time before
        // an entities "moves left" is set by the update. Consider making updates just set positions/health?

        // sendGameStateUpdatesToClients();
    });

    loadMap();
    loadGame();
}

void ServerApplication::run(void) {
    application->run();
}

void ServerApplication::onClientConnect(int clientIndex) {
    auto& context = application->getContext();

    auto player = addPlayer(false);
    auto participantId = context.getTurnController()->addParticipant(true, { player })->id;
    context.getTurnController()->reset();

    // TODO: Set this up so players are assigned properly.
    // Currently participant "0" is the player
    dynamic_cast<ServerTurnController*>(context.getTurnController())->attachParticipantToClient(participantId, clientIndex);

    for(auto& participant : context.getTurnController()->getParticipants()) {
        transmitter->sendSetParticipant(clientIndex, participant);
    }
    
    sendLoadMapToClient(clientIndex);
}

void ServerApplication::sendLoadMapToClient(int clientIndex) {
    auto& context = application->getContext();
    auto grid = context.getGrid();

    auto gridSize = grid->getWidth() * grid->getHeight();
    auto numSequences = gridSize / MaxMapBlockSize;
    auto lastSequence = gridSize % (std::max(numSequences, 1) * MaxMapBlockSize);
    if(lastSequence > 0) {
        numSequences++;
    }

    for(int i = 0; i < numSequences; i++) {
        MapBlock block;
        block.width = grid->getWidth();
        block.height = grid->getHeight();
        block.sequence = i;
        block.numSequences = numSequences;
        block.totalSize = gridSize;
        block.blockSize = MaxMapBlockSize;

        if(lastSequence > 0 && i == numSequences - 1) {
            block.blockSize = lastSequence;
        }

        auto offset = i * MaxMapBlockSize;

        for(int j = 0; j < block.blockSize; j++) {
            auto x = (j + offset) / grid->getWidth();
            auto y = (j + offset) % grid->getHeight();

            block.data[j] = grid->getTileAt(x, y).id;
        }

        std::cout 
            << "Sending map block sequence [" 
            << (i + 1)
            << "] of [" 
            << numSequences
            << "] to client: [" 
            << clientIndex 
            << "]"
            << std::endl;

        transmitter->sendLoadMap(clientIndex, block);
    }

    sendGameStateUpdatesToClients();
}

void ServerApplication::sendGameStateUpdatesToClients(void) {
    auto& context = application->getContext();
    auto currentParticipantId = context.getTurnController()->getCurrentParticipant();
    auto allEntities = context.getEntityPool()->getEntities();
    std::vector<Entity*> entitiesBlock;

    for(auto entity : allEntities) {
        entitiesBlock.push_back(entity);

        if(entitiesBlock.size() == MaxEntities) {
            transmitter->sendGameStateUpdate(
                0, 
                GameStateUpdate::serialize(currentParticipantId, entitiesBlock)
            );
            entitiesBlock.clear();
        }
    }

    if(!entitiesBlock.empty()) {
        transmitter->sendGameStateUpdate(
            0, 
            GameStateUpdate::serialize(currentParticipantId, entitiesBlock)
        );
    }

    // std::cout << "Sent GameStateUpdate" << std::endl;
}

void ServerApplication::loadMap(void) {
    auto& context = application->getContext();
    auto grid = context.getGrid();
    // grid->setData(EmptyStrategy(grid->getWidth(), grid->getHeight()).generate());
    // grid->setData(
    //     HallStrategy(
    //         grid->getWidth(),
    //         grid->getHeight(),
    //         5000,
    //         22
    //     ).generate()
    // );
    grid->setData(WaveFunctionCollapseStrategy(
        grid,
        TileSet("../assets/data/tilesets/grass_and_rocks/rules.json"),
        { 20, glm::ivec2(6, 6), glm::ivec2(15, 15) }
    ).generate());
}

void ServerApplication::loadGame(void) {
    auto& context = application->getContext();

    // auto player = addPlayer(false);
    // auto player2 = addPlayer(true);

    auto numEnemies = randomRange(8, 12);
    std::vector<Entity*> enemies;

    for(int i = 0; i < numEnemies; i++) {
        Entity* enemy;
        auto entityType = randomRange(0, 10);
        auto isWalkable = 1000;
        int x = 0;
        int y = 0;

        while(isWalkable > 0) {
            // x = randomRange(0, 9);
            // y = randomRange(context.getGrid()->getHeight() - 6, context.getGrid()->getHeight() - 1);
            x = randomRange(0, 5);
            y = randomRange(0, 5);
            auto tile = context.getGrid()->getTileAt(x, y);
            isWalkable = tile.isWalkable ? 0 : isWalkable - 1;
        }

        if(entityType > 2) {
            enemy = context.getEntityPool()->addEntity("Space Worm");
            enemy->setPosition(glm::ivec2(x, y));
            auto teeth = context.getWeaponController()->createWeapon("Space Worm Teeth", enemy);
            auto poisonSpit = context.getWeaponController()->createWeapon("Poison Spit", enemy);
            auto teethId = enemy->addWeapon(std::move(teeth))->getId();
            enemy->addWeapon(std::move(poisonSpit));
            enemy->setCurrentWeapon(teethId);
        }
        else {
            enemy = context.getEntityPool()->addEntity("Brute");
            enemy->setPosition(glm::ivec2(x, y));
            auto fists = context.getWeaponController()->createWeapon("Brute Fists", enemy);
            auto fistsId = enemy->addWeapon(std::move(fists))->getId();
            enemy->setCurrentWeapon(fistsId);
        }

        enemies.push_back(enemy);
    }

    // context.getTurnController()->addParticipant(true, { player });
    context.getTurnController()->addParticipant(false, enemies, std::make_unique<ChaseAndAttackStrategy>(context));
    // context.getTurnController()->addParticipant(2, false, { enemy3 }, std::make_unique<ChaseAndAttackStrategy>(2));
    context.getTurnController()->reset();
}

Entity* ServerApplication::addPlayer(bool hasFreezeGun) {
    auto& context = application->getContext();
    Entity* player;

    auto isWalkable = 1000;
    int x = 0;
    int y = 0;

    while(isWalkable > 0) {
        // x = randomRange(0, 5);
        // y = randomRange(0, 5);
        x = randomRange(2, 19);
        y = randomRange(context.getGrid()->getHeight() - 16, context.getGrid()->getHeight() - 1);
        auto tile = context.getGrid()->getTileAt(x, y);
        isWalkable = tile.isWalkable ? 0 : isWalkable - 1;
    }

    if(hasFreezeGun) {
        player = context.getEntityPool()->addEntity("Player FreezeGun");
        player->setPosition(glm::ivec2(x, y));
        auto freezeGun = player->addWeapon(context.getWeaponController()->createWeapon("Freeze Gun", player));
        player->setCurrentWeapon(freezeGun->getId());
    }
    else {
        player = context.getEntityPool()->addEntity("Player");
        player->setPosition(glm::ivec2(x, y));
        auto grenadeLauncher = player->addWeapon(context.getWeaponController()->createWeapon("Grenade Launcher", player));
        player->setCurrentWeapon(grenadeLauncher->getId());
    }
    return player;
}