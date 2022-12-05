#pragma once

#include "core/net/yojimboimport.h"

class ClientMessagesReceiver {
public:
    virtual void receiveMessage(yojimbo::Message* message) = 0;
};