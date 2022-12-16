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

    auto context = Application::getContext();
    entityPool = context->getEntityPool();
    projectilePool = context->getProjectilePool();
    areaOfEffectPool = context->getAreaOfEffectPool();
    turnController = context->getTurnController();

    server = std::make_shared<GameServer>(yojimbo::Address("127.0.0.1", 8081));

    receiver = std::make_shared<GameServerMessagesReceiver>(context);
    transmitter = std::make_shared<GameServerMessagesTransmitter>(server, 
        [&](int clientIndex) {
            onClientConnect(clientIndex);
        }
    );

    server->setReceiver(receiver);
    server->setTransmitter(transmitter);
    context->setServerMessagesTransmitter(transmitter);

    context->getWindow()->addLoopLogicWorker([&](auto const& timeSinceLastFrame, auto& quit) {
        server->update(timeSinceLastFrame);
        turnController->update(timeSinceLastFrame);
        entityPool->updateEntities(timeSinceLastFrame, quit);
        projectilePool->update(timeSinceLastFrame);
        areaOfEffectPool->update(timeSinceLastFrame);
    });

    // TODO: This somehow makes the game state messages get received first rather than the set participant ones.
    // On the client side we need to wait until participants are all set before we load the rest of the map
    context->getTurnController()->addOnNextTurnFunction([&](auto const& currentParticipant, auto const& turnNumber) {
        sendGameStateUpdatesToClients();
    });

    loadMap();
    loadGame();
}

void ServerApplication::run(void) {
    Application::instance().run();
}

void ServerApplication::onClientConnect(const int& clientIndex) {
    auto context = Application::getContext();

    // TODO: Set this up so players are assigned properly.
    // Currently participant "0" is the player
    participantToClientIndex[0] = clientIndex;

    for(auto [_, participant] : context->getTurnController()->getParticipants()) {
        transmitter->sendSetParticipant(clientIndex, participant);
    }
    
    sendLoadMapToClient(clientIndex);
}

void ServerApplication::sendLoadMapToClient(const int& clientIndex) {
    auto context = Application::getContext();
    auto grid = context->getGrid();

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

        transmitter->sendLoadMap(clientIndex, block);
    }

    sendGameStateUpdatesToClients();
}

void ServerApplication::sendGameStateUpdatesToClients(void) {
    auto context = Application::getContext();
    auto currentParticipantId = context->getTurnController()->getCurrentParticipant();
    auto allEntities = context->getEntityPool()->getEntities();
    std::map<uint32_t, std::shared_ptr<Entity>> entitiesBlock;

    for(auto [entityId, entity] : allEntities) {
        entitiesBlock[entityId] = entity;

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
    auto context = Application::getContext();
    auto grid = context->getGrid();

    for(auto i = 0; i < grid->getWidth(); i++) {
        for(auto j = 0; j < grid->getHeight(); j++) {
            if(i == 10 && j != 12) {
                grid->setTile(i, j, { 2, false });
            }
            else {
                grid->setTile(i, j, { 1, true });
            }
        }
    }
}

void ServerApplication::loadGame(void) {
    auto context = Application::getContext();

    auto player = addPlayer(glm::ivec2(0, 0));
    auto player2 = addPlayer(glm::ivec2(2, 1));

    // TODO: Weapon blueprints
    auto enemy = context->getEntityPool()->addEntity("Space Worm");
    enemy->setPosition(glm::ivec2(0, context->getGrid()->getHeight() - 1));
    auto teeth = context->getWeaponController()->createWeapon("Space Worm Teeth", enemy);
    enemy->addWeapon(teeth);
    enemy->setCurrentWeapon(teeth);
    enemy->setBehaviourStrategy(std::make_shared<ChaseAndAttackStrategy>(enemy));

    auto enemy2 = context->getEntityPool()->addEntity("Space Worm");
    enemy2->setPosition(glm::ivec2(5, context->getGrid()->getHeight() - 3));
    auto teeth2 = context->getWeaponController()->createWeapon("Space Worm Teeth", enemy2);
    enemy2->addWeapon(teeth2);
    enemy2->setCurrentWeapon(teeth2);
    enemy2->setBehaviourStrategy(std::make_shared<ChaseAndAttackStrategy>(enemy2));

    auto enemy3 = context->getEntityPool()->addEntity("Space Worm");
    enemy3->setPosition(glm::ivec2(17, context->getGrid()->getHeight() - 3));
    auto teeth3 = context->getWeaponController()->createWeapon("Space Worm Teeth", enemy3);
    enemy3->addWeapon(teeth3);
    enemy3->setCurrentWeapon(teeth3);
    enemy3->setBehaviourStrategy(std::make_shared<ChaseAndAttackStrategy>(enemy3));

    context->getTurnController()->addParticipant(0, { player, player2 }, true);
    context->getTurnController()->addParticipant(1, { enemy, enemy2 }, false);
    context->getTurnController()->addParticipant(2, { enemy3 }, false);
    context->getTurnController()->reset();
}

std::shared_ptr<Entity> ServerApplication::addPlayer(glm::ivec2 position) {
    auto context = Application::getContext();

    auto player = context->getEntityPool()->addEntity("Player");
    player->setPosition(position);
    auto grenadeLauncher = player->addWeapon(context->getWeaponController()->createWeapon("Grenade Launcher", player));
    player->setCurrentWeapon(grenadeLauncher);
    return player;
}