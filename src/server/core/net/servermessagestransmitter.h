#pragma once

class ServerMessagesTransmitter {
public:
    virtual void onClientConnected(int clientIndex) = 0;
};