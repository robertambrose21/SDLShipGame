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
        std::make_unique<Grid>(128, 128),
        std::make_unique<EntityPool>(),
        std::make_unique<WeaponController>(),
        std::make_unique<ProjectilePool>(),
        std::make_unique<AreaOfEffectPool>(),
        std::make_unique<ServerTurnController>(),
        std::make_unique<ItemController>(),
        std::make_unique<EffectController>(),
        std::make_unique<SpawnController>(),
        std::make_unique<VisiblityController>()
    );

    auto& context = application->getContext();

    context.getTurnController()->initialise(application->getContext());
    context.getAreaOfEffectPool()->initialise(application->getContext());
    context.getProjectilePool()->initialise(application->getContext());
    context.getWeaponController()->initialise(application->getContext());
    context.getEntityPool()->initialise(application->getContext());
    context.getItemController()->initialise(application->getContext());
    context.getSpawnController()->initialise(application->getContext());
    context.getVisibilityController()->initialise(application->getContext());

    context.getTurnController()->subscribe<TurnEventData>(&stdoutSubscriber);
    context.getEntityPool()->subscribe<EntityEventData>(&stdoutSubscriber);
    context.getWeaponController()->subscribe<MeleeWeaponEventData>(&stdoutSubscriber);
    context.getProjectilePool()->subscribe<ProjectileEventData>(&stdoutSubscriber);
    context.getAreaOfEffectPool()->subscribe<AreaOfEffectEventData>(&stdoutSubscriber);
    context.getItemController()->subscribe<ItemEventData>(&stdoutSubscriber);
    context.getTurnController()->subscribe<TakeItemActionEventData>(&stdoutSubscriber);
    context.getTurnController()->subscribe<EngagementEventData>(&stdoutSubscriber);
    context.getTurnController()->subscribe<EquipItemActionEventData>(&stdoutSubscriber);

    server = std::make_unique<GameServer>(
        std::make_unique<GameMessageLogger>("server_messages.log"),
        yojimbo::Address("127.0.0.1", 8081)
    );

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
    context.getVisibilityController()->subscribe<TilesRevealedEventData>(transmitter.get());
    context.getEntityPool()->subscribe<EntitySetPositionEventData>(context.getVisibilityController());
    context.getEntityPool()->subscribe<EntitySetPositionEventData>(transmitter.get());
    context.getVisibilityController()->subscribe<EntityVisibilityToParticipantData>(transmitter.get());

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

    auto rooms = loadMap();
    loadGame(rooms);
}

void ServerApplication::run(void) {
    application->run();
}

void ServerApplication::onClientConnect(int clientIndex) {
    auto& context = application->getContext();

    auto clientParticipant = context.getTurnController()->addParticipant(true, { addPlayer(false) });
    context.getTurnController()->reset();

    dynamic_cast<ServerTurnController*>(context.getTurnController())
        ->attachParticipantToClient(clientParticipant->getId(), clientIndex);

    for(auto& participant : context.getTurnController()->getParticipants()) {
        transmitter->sendSetParticipant(clientIndex, participant);
    }
    
    sendLoadMapToClient(clientIndex);

    // Temp hack to trigger a grid tile reveal
    for(auto entity : clientParticipant->getEntities()) {
        entity->setPosition(entity->getPosition());
    }
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

    auto chunkId = getNewId();

    auto currentParticipantId = context.getTurnController()->getCurrentParticipant();
    auto allEntities = context.getEntityPool()->getEntities();
    std::vector<Entity*> entitiesBlock;

    // TODO: Handle overflow?
    uint8_t expectedNumChunks = allEntities.size() / MaxEntities;
    if(allEntities.size() % MaxEntities > 0) {
        expectedNumChunks++;
    }

    for(auto entity : allEntities) {
        if(entity->getCurrentStats().common.hp <= 0) {
            std::cout << "Entity with 0 hp, should not happen" << std::endl;
        }

        entitiesBlock.push_back(entity);

        if(entitiesBlock.size() == MaxEntities) {
            transmitter->sendGameStateUpdate(
                0, 
                GameStateUpdate::serialize(currentParticipantId, entitiesBlock, chunkId, expectedNumChunks)
            );
            std::cout << "Sent GameStateUpdate ["  << entitiesBlock.size() << "]" << std::endl;
            entitiesBlock.clear();
        }
    }

    if(!entitiesBlock.empty()) {
        transmitter->sendGameStateUpdate(
            0, 
            GameStateUpdate::serialize(currentParticipantId, entitiesBlock, chunkId, expectedNumChunks)
        );
        std::cout << "Sent GameStateUpdate ["  << entitiesBlock.size() << "]" << std::endl;
    }

    std::cout << "Total entities " << allEntities.size() << std::endl;
}

std::vector<GenerationStrategy::Room> ServerApplication::loadMap(void) {
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

    auto wfc = WaveFunctionCollapseStrategy(
        grid,
        TileSet("../assets/data/tilesets/grass_and_rocks/rules.json"),
        { 12, glm::ivec2(6, 6), glm::ivec2(15, 15), 1 }
    );

    grid->setData(wfc.generate());

    return wfc.getRooms();
}

// TODO: Eventually move to some kind of map generator class
void ServerApplication::loadGame(const std::vector<GenerationStrategy::Room>& rooms) {
    auto& context = application->getContext();
    std::vector<Entity*> enemies;

    for(auto& room : rooms) {
        if(randomD6() > 3) {
            unfilledRooms.push_back(room);
            continue;
        }

        auto entities = context.getSpawnController()->spawnEntities(
            {
                {
                    { "Space Worm", { "Space Worm Teeth", "Poison Spit" } },
                    { "Brute", { "Brute Fists" } },
                },
                { 5, 1 },
            },
            { room.min, room.max },
            randomRange(8, 12)
        );

        for(auto entity : entities) {
            enemies.push_back(entity);
        }
    }

    context.getTurnController()->addParticipant(false, enemies, std::make_unique<ChaseAndAttackStrategy>(context));
    context.getTurnController()->reset();
}

Entity* ServerApplication::addPlayer(bool hasFreezeGun) {
    auto& context = application->getContext();

    auto room = randomChoice(unfilledRooms);
    auto entities = context.getSpawnController()->spawnEntities(
        {
            {
                { "Player", { "Grenade Launcher" } }
            }
        },
        { room.min, room.max }
    );

    return entities.front();


    // Entity* player;

    // auto isWalkable = 1000;
    // int x = 0;
    // int y = 0;

    // while(isWalkable > 0) {
    //     // x = randomRange(0, 5);
    //     // y = randomRange(0, 5);
    //     x = randomRange(2, 19);
    //     y = randomRange(context.getGrid()->getHeight() - 16, context.getGrid()->getHeight() - 1);
    //     auto tile = context.getGrid()->getTileAt(x, y);
    //     isWalkable = tile.isWalkable ? 0 : isWalkable - 1;
    // }

    // if(hasFreezeGun) {
    //     player = context.getEntityPool()->addEntity("Player FreezeGun");
    //     player->setPosition(glm::ivec2(x, y));
    //     auto freezeGun = player->addWeapon(context.getWeaponController()->createWeapon("Freeze Gun", player));
    //     player->setCurrentWeapon(freezeGun->getId());
    // }
    // else {
    //     player = context.getEntityPool()->addEntity("Player");
    //     player->setPosition(glm::ivec2(x, y));
    //     auto grenadeLauncher = player->addWeapon(context.getWeaponController()->createWeapon("Grenade Launcher", player));
    //     player->setCurrentWeapon(grenadeLauncher->getId());
    // }

    // return player;
}