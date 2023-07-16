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

    auto& context = Application::getContext();
    auto& entityPool = context.getEntityPool();
    auto& projectilePool = context.getProjectilePool();
    auto& areaOfEffectPool = context.getAreaOfEffectPool();
    auto& turnController = context.getTurnController();

    server = std::make_unique<GameServer>(yojimbo::Address("127.0.0.1", 8081));

    receiver = std::make_unique<GameServerMessagesReceiver>(context);
    transmitter = std::make_unique<GameServerMessagesTransmitter>(*server, 
        [&](int clientIndex) {
            onClientConnect(clientIndex);
        }
    );
    receiver->setTransmitter(transmitter.get());

    server->setReceiver(receiver.get());
    server->setTransmitter(transmitter.get());
    context.setServerMessagesTransmitter(transmitter.get());

    context.getWindow().addLoopLogicWorker([&](auto const& timeSinceLastFrame, auto& quit) {
        server->update(timeSinceLastFrame);
        turnController.update(timeSinceLastFrame, quit);
        entityPool.updateEntities(timeSinceLastFrame, quit);
        projectilePool.update(timeSinceLastFrame);
        areaOfEffectPool.update(timeSinceLastFrame);
    });

    // TODO: This somehow makes the game state messages get received first rather than the set participant ones.
    // On the client side we need to wait until participants are all set before we load the rest of the map
    context.getTurnController().addOnNextTurnFunction([&](auto const& currentParticipant, auto const& turnNumber) {
        sendGameStateUpdatesToClients();
    });

    loadMap();
    loadGame();
}

void ServerApplication::run(void) {
    Application::instance().run();
}

void ServerApplication::onClientConnect(int clientIndex) {
    auto& context = Application::getContext();

    // TODO: Set this up so players are assigned properly.
    // Currently participant "0" is the player
    participantToClientIndex[0] = clientIndex;

    for(auto& participant : context.getTurnController().getParticipants()) {
        transmitter->sendSetParticipant(clientIndex, participant);
    }
    
    sendLoadMapToClient(clientIndex);
}

void ServerApplication::sendLoadMapToClient(int clientIndex) {
    auto& context = Application::getContext();
    auto& grid = context.getGrid();

    auto gridSize = grid.getWidth() * grid.getHeight();
    auto numSequences = gridSize / MaxMapBlockSize;
    auto lastSequence = gridSize % (std::max(numSequences, 1) * MaxMapBlockSize);
    if(lastSequence > 0) {
        numSequences++;
    }

    for(int i = 0; i < numSequences; i++) {
        MapBlock block;
        block.width = grid.getWidth();
        block.height = grid.getHeight();
        block.sequence = i;
        block.numSequences = numSequences;
        block.totalSize = gridSize;
        block.blockSize = MaxMapBlockSize;

        if(lastSequence > 0 && i == numSequences - 1) {
            block.blockSize = lastSequence;
        }

        auto offset = i * MaxMapBlockSize;

        for(int j = 0; j < block.blockSize; j++) {
            auto x = (j + offset) / grid.getWidth();
            auto y = (j + offset) % grid.getHeight();

            block.data[j] = grid.getTileAt(x, y).id;
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
    auto& context = Application::getContext();
    auto currentParticipantId = context.getTurnController().getCurrentParticipant();
    auto allEntities = context.getEntityPool().getEntities();
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
}

void ServerApplication::loadMap(void) {
    auto& context = Application::getContext();
    auto& grid = context.getGrid();

    for(auto i = 0; i < grid.getWidth(); i++) {
        for(auto j = 0; j < grid.getHeight(); j++) {
            if(i == 10 && j != 12) {
                grid.setTile(i, j, { 2, false });
            }
            else {
                grid.setTile(i, j, { 1, true });
            }
        }
    }
}

void ServerApplication::loadGame(void) {
    auto& context = Application::getContext();

    auto player = addPlayer(glm::ivec2(0, 0), false);
    auto player2 = addPlayer(glm::ivec2(2, 1), true);

    auto numEnemies = randomRange(8, 12);
    std::set<Entity*> enemies;

    for(int i = 0; i < numEnemies; i++) {
        Entity* enemy;
        auto entityType = randomRange(0, 10);
        auto x = randomRange(0, 9);
        auto y = randomRange(context.getGrid().getHeight() - 6, context.getGrid().getHeight() - 1);

        if(entityType > 2) {
            enemy = context.getEntityPool().addEntity("Space Worm");
            enemy->setPosition(glm::ivec2(x, y));
            auto teeth = context.getWeaponController().createWeapon("Space Worm Teeth", enemy);
            auto poisonSpit = context.getWeaponController().createWeapon("Poison Spit", enemy);
            auto teethId = enemy->addWeapon(std::move(teeth))->getId();
            enemy->addWeapon(std::move(poisonSpit));
            enemy->setCurrentWeapon(teethId);
        }
        else {
            enemy = context.getEntityPool().addEntity("Brute");
            enemy->setPosition(glm::ivec2(x, y));
            auto fists = context.getWeaponController().createWeapon("Brute Fists", enemy);
            auto fistsId = enemy->addWeapon(std::move(fists))->getId();
            enemy->setCurrentWeapon(fistsId);
        }

        enemies.insert(enemy);
    }
    context.getTurnController().addParticipant(0, true, { player, player2 });
    context.getTurnController().addParticipant(1, false, enemies, std::make_unique<ChaseAndAttackStrategy>(1));
    // context.getTurnController().addParticipant(2, false, { enemy3 }, std::make_unique<ChaseAndAttackStrategy>(2));
    context.getTurnController().reset();
}

Entity* ServerApplication::addPlayer(glm::ivec2 position, bool hasFreezeGun) {
    auto& context = Application::getContext();
    Entity* player;

    if(hasFreezeGun) {
        player = context.getEntityPool().addEntity("Player FreezeGun");
        player->setPosition(position);
        auto freezeGun = player->addWeapon(context.getWeaponController().createWeapon("Freeze Gun", player));
        player->setCurrentWeapon(freezeGun->getId());
    }
    else {
        player = context.getEntityPool().addEntity("Player");
        player->setPosition(position);
        auto grenadeLauncher = player->addWeapon(context.getWeaponController().createWeapon("Grenade Launcher", player));
        player->setCurrentWeapon(grenadeLauncher->getId());
    }
    return player;
}