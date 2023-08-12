#pragma once

#include "core/net/yojimboimport.h"

class ServerMessagesReceiver {
public:
    virtual void receiveMessage(int participantId, yojimbo::Message* message) = 0;
};