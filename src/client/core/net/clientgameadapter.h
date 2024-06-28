#pragma once

#include <memory>

#include "core/net/yojimboimport.h"
#include "game/message/messages.h"

static const uint8_t DEFAULT_PRIVATE_KEY_CLIENT[yojimbo::KeyBytes] = { 0 };

// two channels, one for each type that Yojimbo supports
enum class ClientGameChannel {
    RELIABLE,
    UNRELIABLE,
    COUNT
};

// the client and server config
struct ClientGameConnectionConfig : yojimbo::ClientServerConfig {
    ClientGameConnectionConfig()  {
        numChannels = 2;
        channel[(int)ClientGameChannel::RELIABLE].type = yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED;
        channel[(int)ClientGameChannel::UNRELIABLE].type = yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED;
    }
};

class ClientGameAdapter : public yojimbo::Adapter {
public:
    explicit ClientGameAdapter() { }
    ~ClientGameAdapter();

    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override;

    void OnServerClientConnected(int clientIndex) override;

    void OnServerClientDisconnected(int clientIndex) override;
};