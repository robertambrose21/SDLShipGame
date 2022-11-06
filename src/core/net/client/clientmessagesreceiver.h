#pragma once

#include "yojimbo/yojimbo.h"

class ClientMessagesReceiver {
public:
    virtual void receiveMessage(yojimbo::Message* message) = 0;
};