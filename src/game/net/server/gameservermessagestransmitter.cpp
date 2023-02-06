#include "gameservermessagestransmitter.h"

GameServerMessagesTransmitter::GameServerMessagesTransmitter(
    std::shared_ptr<GameServer> server,
    std::function<void(int)> onClientConnectFunc
) :
    server(server),
    onClientConnectFunc(onClientConnectFunc)
{
    turnController = Application::getContext()->getTurnController();
}

void GameServerMessagesTransmitter::onClientConnected(int clientIndex) {
    onClientConnectFunc(clientIndex);
}

void GameServerMessagesTransmitter::sendSetParticipant(
    int clientIndex, 
    const std::shared_ptr<TurnController::Participant>& participant
) {
    SetParticipantMessage* message = (SetParticipantMessage*) server->createMessage(clientIndex, GameMessageType::SET_PARTICIPANT);

    message->participantId = participant->id;
    message->numParticipantsToSet = turnController->getParticipants().size();
    message->isPlayer = participant->isPlayer;

    server->sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendGameStateUpdate(int clientIndex, const GameStateUpdate& update) {
    GameStateUpdateMessage* message = (GameStateUpdateMessage*) server->createMessage(clientIndex, GameMessageType::GAME_STATE_UPDATE);

    message->gameStateUpdate = update;

    server->sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendLoadMap(int clientIndex, const MapBlock& block) {
    LoadMapMessage* message = (LoadMapMessage*) server->createMessage(clientIndex, GameMessageType::LOAD_MAP);

    message->mapBlock = block;

    server->sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendFindPath(
    int clientIndex, 
    uint32_t entityId, 
    const glm::ivec2& position,
    int shortStopSteps
) {
    FindPathMessage* message = (FindPathMessage*) server->createMessage(clientIndex, GameMessageType::FIND_PATH);

    message->entityId = entityId;
    message->x = position.x;
    message->y = position.y;
    message->shortStopSteps = shortStopSteps;

    server->sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendAttackEntity(
    int clientIndex,
    uint32_t entityId, 
    uint32_t targetId, 
    uint32_t weaponId
) {
    AttackEntityMessage* message = (AttackEntityMessage*) server->createMessage(clientIndex, GameMessageType::ATTACK_ENTITY);

    message->entityId = entityId;
    message->targetId = targetId;
    message->weaponId = weaponId;

    server->sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendActionsRollResponse(int clientIndex, int participantId, const std::vector<DiceActionResult>& dice) {
    ActionsRollResponseMessage* message = 
        (ActionsRollResponseMessage*) server->createMessage(clientIndex, GameMessageType::ACTIONS_ROLL_RESPONSE);

    message->participantId = participantId;
    message->numDice = dice.size();
    for(int i = 0; i < dice.size(); i++) {
        message->dice[i] = dice[i];
    }

    server->sendMessage(clientIndex, message);
}