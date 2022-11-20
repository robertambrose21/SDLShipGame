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

    receiver = std::make_shared<GameServerMessagesReceiver>(context);
    server = std::make_shared<GameServer>(receiver, yojimbo::Address("127.0.0.1", 8081));
    transmitter = std::make_shared<GameServerMessagesTransmitter>(server, 
        [&](int clientIndex) {
            participantToClientIndex[0] = clientIndex;
            transmitter->sendSetParticipant(clientIndex, 0);
            sendLoadMapToClient(clientIndex);
        }
    );
    
    server->setTransmitter(transmitter);

    context->getWindow()->addLoopLogicWorker([&](auto timeSinceLastFrame, auto& quit) {
        server->update(timeSinceLastFrame);
    });

    context->getTurnController()->addOnNextTurnFunction([&](int currentParticipant, int turnNumber) {
        if(currentParticipant == 0) {
            transmitter->sendGameStateUpdate(
                participantToClientIndex[currentParticipant], 
                context->getCurrentGameState()
            );
        }
    });

    loadMap();
}

void ServerApplication::run(void) {
    Application::instance().run();
}

void ServerApplication::sendLoadMapToClient(int clientIndex) {
    auto grid = Application::getContext()->getGrid();

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
    auto enemy = context->getEntityPool()->addEntity(
        std::make_shared<Entity>(
            "Space Worm", 
            Entity::Stats { 5, 2 }
    ));
    enemy->setPosition(glm::ivec2(0, context->getGrid()->getHeight() - 1));
    enemy->setTextureId(7);
    enemy->setSelectedTextureId(6);
    auto teeth = std::make_shared<MeleeWeapon>(enemy, context->getGrid(), "Teeth", (Weapon::Stats) { 1, 2, 1 });
    enemy->addWeapon(teeth);
    enemy->setCurrentWeapon(teeth);
    enemy->setBehaviourStrategy(std::make_shared<ChaseAndAttackStrategy>(enemy));

    auto enemy2 = context->getEntityPool()->addEntity(
        std::make_shared<Entity>(
            "Space Worm", 
            Entity::Stats { 5, 2 }
    ));
    enemy2->setPosition(glm::ivec2(5, context->getGrid()->getHeight() - 3));
    enemy2->setTextureId(7);
    enemy2->setSelectedTextureId(6);
    auto teeth2 = std::make_shared<MeleeWeapon>(enemy2, context->getGrid(), "Teeth", (Weapon::Stats) { 1, 2, 1 });
    enemy->addWeapon(teeth2);
    enemy2->setCurrentWeapon(teeth2);
    enemy2->setBehaviourStrategy(std::make_shared<ChaseAndAttackStrategy>(enemy2));

    auto enemy3 = context->getEntityPool()->addEntity(
        std::make_shared<Entity>(
            "Space Worm", 
            Entity::Stats { 5, 2 }
    ));
    enemy3->setPosition(glm::ivec2(17, context->getGrid()->getHeight() - 3));
    enemy3->setTextureId(7);
    enemy3->setSelectedTextureId(6);
    auto teeth3 = std::make_shared<MeleeWeapon>(enemy3, context->getGrid(), "Teeth", (Weapon::Stats) { 1, 2, 1 });
    enemy3->addWeapon(teeth3);
    enemy3->setCurrentWeapon(teeth3);
    enemy3->setBehaviourStrategy(std::make_shared<ChaseAndAttackStrategy>(enemy3));

    context->getTurnController()->addParticipant({ player, player2 }, true);
    context->getTurnController()->addParticipant({ enemy, enemy2 }, false);
    context->getTurnController()->addParticipant({ enemy3 }, false);
    context->getTurnController()->reset();

    context->getWindow()->addLoopDrawWorker([&](auto graphicsContext, auto& quit) {
        context->getEntityPool()->drawEntities(graphicsContext);
        context->getProjectilePool()->draw(graphicsContext);
        context->getAreaOfEffectPool()->draw(graphicsContext);
    });
    context->getWindow()->addLoopLogicWorker([&](auto timeSinceLastFrame, auto& quit) {
        context->getTurnController()->update(timeSinceLastFrame);
        context->getEntityPool()->updateEntities(timeSinceLastFrame, quit);
        context->getProjectilePool()->update(timeSinceLastFrame);
        context->getAreaOfEffectPool()->update(timeSinceLastFrame);
    });

    context->getWindow()->setGridTileTexture(1, 4);
    context->getWindow()->setGridTileTexture(2, 5);
}

std::shared_ptr<Entity> ServerApplication::addPlayer(glm::ivec2 position) {
    auto context = Application::getContext();

    auto player = context->getEntityPool()->addEntity(std::make_shared<Entity>("Player", Entity::Stats { 3, 10 }));
    player->setTextureId(1);
    player->setSelectedTextureId(6);
    player->setPosition(position);
    
    auto pistolTemp = std::make_shared<ProjectileWeapon>(
        player,
        context->getWindow()->getGridRenderer()->getGrid(), 
        "Pistol", 
        Weapon::Stats { 1, 100, 2 },
        Projectile::Blueprint(
            Projectile::Stats { 1, 50 },
            2,
            [&](auto grid, auto entity, auto turnNumber) {
                context->getAreaOfEffectPool()->add(std::make_shared<AreaOfEffect>(
                    context->getWindow()->getGridRenderer()->getGrid(), 
                    3,
                    turnNumber,
                    entity->getPosition(),
                    AreaOfEffect::Stats { 2.0f, 1 }
                ));
            }
        )
        
    );
    auto pistol = player->addWeapon(pistolTemp);
    player->setCurrentWeapon(pistol);
    return player;
}