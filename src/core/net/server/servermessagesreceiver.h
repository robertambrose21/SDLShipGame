#pragma once

#include "yojimbo/yojimbo.h"

class ServerMessagesReceiver {
public:
    virtual void receiveMessage(const int& participantId, yojimbo::Message* message) = 0;
};