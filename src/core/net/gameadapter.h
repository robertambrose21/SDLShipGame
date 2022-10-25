#pragma once

#include <memory>

#include "yojimbo/yojimbo.h"
#include "yojimbo/shared.h"

static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

enum class GameMessageType {
    TEST,
    COUNT
};

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

class TestMessage2 : public yojimbo::Message {
public:
    int m_data;

    TestMessage2() :
        m_data(0) {}

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, m_data, 0, 512);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::TEST, TestMessage2);
YOJIMBO_MESSAGE_FACTORY_FINISH();

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