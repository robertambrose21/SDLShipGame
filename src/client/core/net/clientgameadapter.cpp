#include "clientgameadapter.h"

ClientGameAdapter::~ClientGameAdapter()
{ }

yojimbo::MessageFactory* ClientGameAdapter::CreateMessageFactory(yojimbo::Allocator& allocator) {
    return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
}

void ClientGameAdapter::OnServerClientConnected(int clientIndex) {
    // No-op
}

void ClientGameAdapter::OnServerClientDisconnected(int clientIndex) {
    // No-op
}