#pragma once

#include <memory>

#include "yojimbo/yojimbo.h"
#include "yojimbo/shared.h"

#include "message/messages.h"

static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

// two channels, one for each type that Yojimbo supports
enum class GameChannel {
    RELIABLE,
    UNRELIABLE,
    COUNT
};

// the client and server config
struct GameConnectionConfig : yojimbo::ClientServerConfig {
    GameConnectionConfig()  {
        numChannels = 2;
        channel[(int)GameChannel::RELIABLE].type = yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED;
        channel[(int)GameChannel::UNRELIABLE].type = yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED;
    }
};

class GameServer;

class GameAdapter : public yojimbo::Adapter {
private:
    GameServer* server;

public:
    explicit GameAdapter(GameServer* server = NULL) :
        server(server)
    { }
    ~GameAdapter();

    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override;

    void OnServerClientConnected(int clientIndex) override;

    void OnServerClientDisconnected(int clientIndex) override;
};