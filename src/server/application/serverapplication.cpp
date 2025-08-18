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

    #if !defined(NDEBUG)
        spdlog::set_level(spdlog::level::trace);
        spdlog::debug("Debug logging enabled");
    #else
        spdlog::set_level(spdlog::level::info);
    #endif

    application = std::make_unique<Application>(
        std::make_unique<Grid>(128, 128),
        std::make_unique<ActorPool>(),
        std::make_unique<WeaponController>(),
        std::make_unique<ProjectilePool>(),
        std::make_unique<AreaOfEffectPool>(),
        std::make_unique<ServerGameController>(),
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
    context.getActorPool()->initialise(application->getContext());
    context.getItemController()->initialise(application->getContext());
    context.getSpawnController()->initialise(application->getContext());
    context.getVisibilityController()->initialise(application->getContext());
    context.getEffectController()->initialise(application->getContext());
    context.getActorPool()->subscribe<ActorEventData>(&stdoutSubscriber);
    context.getWeaponController()->subscribe<MeleeWeaponEventData>(&stdoutSubscriber);
    context.getProjectilePool()->subscribe<ProjectileEventData>(&stdoutSubscriber);
    context.getAreaOfEffectPool()->subscribe<AreaOfEffectEventData>(&stdoutSubscriber);
    context.getItemController()->subscribe<ItemEventData>(&stdoutSubscriber);
    context.getGameController()->subscribe<TakeItemActionEventData>(&stdoutSubscriber);
    context.getGameController()->subscribe<EquipItemActionEventData>(&stdoutSubscriber);

    server = std::make_unique<GameServer>(
        std::make_unique<GameMessageLogger>("server_messages.log"),
        yojimbo::Address("127.0.0.1", 8081)
    );

    receiver = std::make_unique<GameServerMessagesReceiver>(application->getContext());
    transmitter = std::make_unique<GameServerMessagesTransmitter>(
        *server, 
        dynamic_cast<ServerGameController*>(context.getGameController()),
        context.getVisibilityController(),
        context.getItemController(),
        [&](int clientIndex) { onClientConnect(clientIndex); },
        [&](int clientIndex) { onClientDisconnect(clientIndex); }
    );
    receiver->setTransmitter(transmitter.get());

    dynamic_cast<ServerGameController*>(context.getGameController())->setTransmitter(transmitter.get());

    server->setReceiver(receiver.get());
    server->setTransmitter(transmitter.get());
    context.setServerMessagesTransmitter(transmitter.get());
    context.getItemController()->subscribe<ItemEventData>(transmitter.get());
    context.getActorPool()->subscribe(context.getItemController());
    context.getGameController()->subscribe<MoveActionEventData>(transmitter.get());
    context.getGameController()->subscribe<AttackActionEventData>(transmitter.get());
    context.getGameController()->subscribe<TakeItemActionEventData>(transmitter.get());
    context.getAreaOfEffectPool()->subscribe<AreaOfEffectEventData>(transmitter.get());
    context.getProjectilePool()->subscribe<ProjectileEventData>(transmitter.get());
    context.getWeaponController()->subscribe<MeleeWeaponEventData>(transmitter.get());
    context.getEffectController()->subscribe<ActorEffectEvent>(transmitter.get());
    context.getEffectController()->subscribe<GridEffectEvent>(transmitter.get());
    context.getVisibilityController()->subscribe<TilesRevealedEventData>(transmitter.get());
    context.getActorPool()->subscribe<ActorSetPositionEventData>(context.getVisibilityController());
    context.getActorPool()->subscribe<ActorSetPositionEventData>(transmitter.get());
    context.getActorPool()->subscribe<ActorSetPositionEventData>(dynamic_cast<ServerGameController*>(context.getGameController()));
    context.getVisibilityController()->subscribe<ActorVisibilityToParticipantData>(transmitter.get());
    context.getGameController()->getEngagementController()->subscribe<CreateEngagementEventData>(transmitter.get());
    context.getGameController()->getEngagementController()->subscribe<AddToEngagementEventData>(transmitter.get());
    context.getGameController()->getEngagementController()->subscribe<DisengageEventData>(transmitter.get());
    context.getGameController()->getEngagementController()->subscribe<RemoveEngagementEventData>(transmitter.get());
    context.getGameController()->getEngagementController()->subscribe<MergeEngagementEventData>(transmitter.get());
    context.getGameController()->getFactionController()->subscribe<CreateFactionEventData>(transmitter.get());
    context.getGameController()->getFactionController()->subscribe<SetFactionEventData>(transmitter.get());
    context.getGameController()->getFactionController()->subscribe<AddFactionEventData>(transmitter.get());
    context.getGameController()->getFactionController()->subscribe<ChangeFactionAlignmentEventData>(transmitter.get());

    application->addLogicWorker([&](ApplicationContext& c, auto const& timeSinceLastFrame, auto& quit) {
        server->update(timeSinceLastFrame);
        c.getGameController()->update(timeSinceLastFrame, quit);
        c.getActorPool()->updateActors(timeSinceLastFrame, quit);
        c.getProjectilePool()->update(timeSinceLastFrame);
        c.getAreaOfEffectPool()->update(timeSinceLastFrame);
        c.getEffectController()->update(timeSinceLastFrame);
    });

    auto rooms = loadMap();
    loadGame(rooms);
}

void ServerApplication::run(void) {
    application->run();
}

void ServerApplication::onClientConnect(int clientIndex) {
    auto gameController = dynamic_cast<ServerGameController*>(application->getContext().getGameController());
    auto factionController = gameController->getFactionController();
    uint64_t clientId = server->getClientId(clientIndex);

    Participant* participant = nullptr;

    if(server->hasReconnected(clientIndex)) {
        auto participantId = gameController->getParticipantByClientId(clientId);

        if(participantId == -1) {
            spdlog::warn("Detected reconnect for client {}:[{}] but cannot find participant", clientIndex, clientId);
            return;
        }

        participant = gameController->getParticipant(participantId);
        spdlog::trace("Client {} reconnected and is attaching to participant {}", clientIndex, participant->getId());
    }
    else {
        participant = gameController->addParticipant(true, { addPlayer(false) });

        transmitter->sendFactionUpdates(clientIndex, factionController->getAlignedFactions());
        auto basedFaction = factionController->getFactionByName("Based");
        auto cringeFaction = factionController->getFactionByName("Cringe");
        factionController->setParticipantFaction(participant, basedFaction->id);
        factionController->addFactionAlignment(participant, cringeFaction->id, Factioned::Faction::HOSTILE);

        gameController->reset(); // TODO: Probably shouldn't do this?
        spdlog::trace("Client {} connected and is attaching to participant {}", clientIndex, participant->getId());
    }

    gameController->attachParticipantToClient(participant->getId(), clientIndex, clientId);
    spdlog::trace("Attached client {}:[{}] to participant {}", clientIndex, clientId, participant->getId());

    // TOOD: Send just unready participants to all clients
    for(auto& p : gameController->getParticipants()) {
        transmitter->sendSetParticipantToAllClients(p);
    }

    // Temp hack to trigger a grid tile reveal
    for(auto actor : participant->getActors()) {
        actor->setPosition(actor->getPosition());
        spdlog::trace(
            "Actor {} spawned at position ({}, {}) for participant {}", 
            actor->toString(), 
            actor->getPosition().x,
            actor->getPosition().y,
            participant->getId()
        );
    }
    
    transmitter->sendLoadGameToClient(clientIndex);
    sendGameStateUpdatesToParticipant(clientIndex);
}

void ServerApplication::onClientDisconnect(int clientIndex) {
    dynamic_cast<ServerGameController*>(application->getContext().getGameController())
        ->detachParticipantFromClient(clientIndex);
}

// TODO: This is not needed when using the fog of war visibility - reserve for when we want to reveal the whole map.
void ServerApplication::sendLoadMapToClient(int clientIndex) {
    auto& context = application->getContext();
    auto grid = context.getGrid();
    auto gameController = dynamic_cast<ServerGameController*>(context.getGameController());

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

        // transmitter->sendLoadMap(clientIndex, block);
    }

    sendGameStateUpdatesToParticipant(gameController->getAttachedParticipantId(clientIndex));
}

// TODO: Move elsewhere
void ServerApplication::sendGameStateUpdatesToParticipant(int clientIndex) {
    auto& context = application->getContext();
    auto gameController = dynamic_cast<ServerGameController*>(context.getGameController());
    auto participantId = gameController->getAttachedParticipantId(clientIndex);

    auto chunkId = getNewId();

    auto visibleActors = context.getGameController()->getParticipant(participantId)->getVisibleActors();
    std::vector<Actor*> actorsBlock;

    // TODO: Handle overflow?
    uint8_t expectedNumChunks = visibleActors.size() / MaxActors;
    if(visibleActors.size() % MaxActors > 0) {
        expectedNumChunks++;
    }

    for(auto actor : visibleActors) {
        if(actor->getStats().hp <= 0) {
            std::cout << "Actor with 0 hp, should not happen" << std::endl;
        }

        actorsBlock.push_back(actor);

        if(actorsBlock.size() == MaxActors) {
            transmitter->sendGameStateUpdate(
                clientIndex, 
                GameStateUpdate::serialize(participantId, actorsBlock, chunkId, expectedNumChunks)
            );
            spdlog::trace("Sent GameStateUpdate [{}] to participant {}", actorsBlock.size(), participantId);
            actorsBlock.clear();
        }
    }

    if(!actorsBlock.empty()) {
        transmitter->sendGameStateUpdate(
            clientIndex, 
            GameStateUpdate::serialize(participantId, actorsBlock, chunkId, expectedNumChunks)
        );
        spdlog::trace("Sent GameStateUpdate [{}] to participant {}", actorsBlock.size(), participantId);
    }

    spdlog::trace("Total visible actors [{}] to participant {}", visibleActors.size(), participantId);
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

    auto tileSet = WFCTileSet("../assets/data/tilesets/grass_and_rocks/rules.json");
    tileSet.load();

    auto wfc = WFCStrategy(
        grid,
        { 12, glm::ivec2(6, 6), glm::ivec2(15, 15), 1 },
        tileSet
    );

    grid->setData(wfc.generate());
    
    return wfc.getRooms();
}

// TODO: Eventually move to some kind of map generator class
void ServerApplication::loadGame(const std::vector<GenerationStrategy::Room>& rooms) {
    auto& context = application->getContext();
    std::vector<Actor*> enemies;

    for(auto& room : rooms) {
        if(randomD6() > 3) {
            unfilledRooms.push_back(room);
            continue;
        }

        auto actors = context.getSpawnController()->spawnActors(
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

        for(auto actor : actors) {
            enemies.push_back(actor);
        }
    }

    auto factionController = context.getGameController()->getFactionController();

    factionController->createFaction("Based");
    factionController->createFaction("Cringe");
    auto basedFaction = factionController->getFactionByName("Based");
    auto cringeFaction = factionController->getFactionByName("Cringe");

    auto participant = context.getGameController()->addParticipant(
        false, 
        enemies, 
        std::make_unique<ChaseAndAttackStrategy>(context)
    );

    factionController->setParticipantFaction(participant, cringeFaction->id);
    factionController->addFactionAlignment(participant, basedFaction->id, Factioned::Faction::HOSTILE);

    context.getGameController()->reset();
}

Actor* ServerApplication::addPlayer(bool hasFreezeGun) {
    auto& context = application->getContext();

    static int i = 0;

    auto room = randomChoice(unfilledRooms);
    SpawnController::SpawnBox spawnBoxA = { { 31, 91 }, { 31, 91 } };
    SpawnController::SpawnBox spawnBoxB = { { 15, 91 }, { 15, 91 } };

    auto actors = context.getSpawnController()->spawnActors(
        {
            {
                { "Player", { "Grenade Launcher" } }
                // { "Player FreezeGun", { "Freeze Gun" } }
            }
        },
        { room.min, room.max }
        // i == 0 ? spawnBoxA : spawnBoxB
    );

    i++;

    return actors.front();

    // Actor* player;

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
    //     player = context.getActorPool()->addActor("Player FreezeGun");
    //     player->setPosition(glm::ivec2(x, y));
    //     auto freezeGun = player->addWeapon(context.getWeaponController()->createWeapon("Freeze Gun", player));
    //     player->setCurrentWeapon(freezeGun->getId());
    // }
    // else {
    //     player = context.getActorPool()->addActor("Player");
    //     player->setPosition(glm::ivec2(x, y));
    //     auto grenadeLauncher = player->addWeapon(context.getWeaponController()->createWeapon("Grenade Launcher", player));
    //     player->setCurrentWeapon(grenadeLauncher->getId());
    // }

    // return player;
}