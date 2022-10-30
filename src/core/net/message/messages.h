#pragma once

#include "yojimbo/yojimbo.h"
#include "yojimbo/shared.h"

enum class GameMessageType {
    FIND_PATH,
    COUNT
};

class FindPathMessage : public yojimbo::Message {
public:
    int x, y;

    FindPathMessage() :
        x(0),
        y(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, x, 0, 512);
        serialize_int(stream, y, 0, 512);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::FIND_PATH, FindPathMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();