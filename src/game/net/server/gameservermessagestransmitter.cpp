#include "gameservermessagestransmitter.h"

GameServerMessagesTransmitter::GameServerMessagesTransmitter(
    std::shared_ptr<GameServer> server,
    std::function<void(int)> onClientConnectFunc
) :
    server(server),
    onClientConnectFunc(onClientConnectFunc)
{ }

void GameServerMessagesTransmitter::onClientConnected(int clientIndex) {
    // TODO: Move to ServerApplication and assign participant id properly
    SetParticipantMessage* message = (SetParticipantMessage*) server->createMessage(clientIndex, GameMessageType::SET_PARTICIPANT);

    message->participantId = 0;

    server->sendMessage(clientIndex, message);

    onClientConnectFunc(clientIndex);
}

void GameServerMessagesTransmitter::sendGameStateUpdate(GameStateUpdate update) {
    GameStateUpdateMessage* message = (GameStateUpdateMessage*) server->createMessage(0, GameMessageType::GAME_STATE_UPDATE);

    message->gameStateUpdate = update;

    server->sendMessage(0, message);
}

void GameServerMessagesTransmitter::sendLoadMap(int clientIndex, const MapBlock& block) {
    LoadMapMessage* message = (LoadMapMessage*) server->createMessage(clientIndex, GameMessageType::LOAD_MAP);

    message->mapBlock = block;

    server->sendMessage(clientIndex, message);
}