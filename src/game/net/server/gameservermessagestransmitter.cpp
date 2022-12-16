#include "gameservermessagestransmitter.h"

GameServerMessagesTransmitter::GameServerMessagesTransmitter(
    std::shared_ptr<GameServer> server,
    std::function<void(const int&)> onClientConnectFunc
) :
    server(server),
    onClientConnectFunc(onClientConnectFunc)
{
    turnController = Application::getContext()->getTurnController();
}

void GameServerMessagesTransmitter::onClientConnected(const int& clientIndex) {
    onClientConnectFunc(clientIndex);
}

void GameServerMessagesTransmitter::sendSetParticipant(
    const int& clientIndex, 
    const std::shared_ptr<TurnController::Participant>& participant
) {
    SetParticipantMessage* message = (SetParticipantMessage*) server->createMessage(clientIndex, GameMessageType::SET_PARTICIPANT);

    message->participantId = participant->id;
    message->numParticipantsToSet = turnController->getParticipants().size();
    message->isPlayer = participant->isPlayer;

    server->sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendGameStateUpdate(const int& clientIndex, const GameStateUpdate& update) {
    GameStateUpdateMessage* message = (GameStateUpdateMessage*) server->createMessage(clientIndex, GameMessageType::GAME_STATE_UPDATE);

    message->gameStateUpdate = update;

    server->sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendLoadMap(const int& clientIndex, const MapBlock& block) {
    LoadMapMessage* message = (LoadMapMessage*) server->createMessage(clientIndex, GameMessageType::LOAD_MAP);

    message->mapBlock = block;

    server->sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendFindPath(
    const int& clientIndex, 
    const uint32_t& entityId, 
    const glm::ivec2& position,
    const int& shortStopSteps
) {
    FindPathMessage* message = (FindPathMessage*) server->createMessage(clientIndex, GameMessageType::FIND_PATH);

    message->entityId = entityId;
    message->x = position.x;
    message->y = position.y;
    message->shortStopSteps = shortStopSteps;

    server->sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendAttackEntity(
    const int& clientIndex,
    const uint32_t& entityId, 
    const uint32_t& targetId, 
    const uint32_t& weaponId
) {
    AttackEntityMessage* message = (AttackEntityMessage*) server->createMessage(clientIndex, GameMessageType::ATTACK_ENTITY);

    message->entityId = entityId;
    message->targetId = targetId;
    message->weaponId = weaponId;

    server->sendMessage(clientIndex, message);
}