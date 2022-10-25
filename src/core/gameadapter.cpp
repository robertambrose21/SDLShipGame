#include "gameadapter.h"
#include "gameserver.h"

GameAdapter::~GameAdapter()
{ }

yojimbo::MessageFactory* GameAdapter::CreateMessageFactory(yojimbo::Allocator& allocator) {
    return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
}

void GameAdapter::OnServerClientConnected(int clientIndex) {
    if(server == nullptr) {
        return;
    }

    server->clientConnected(clientIndex);
}

void GameAdapter::OnServerClientDisconnected(int clientIndex) {
    if(server == nullptr) {
        return;
    }

    server->clientDisconnected(clientIndex);
}