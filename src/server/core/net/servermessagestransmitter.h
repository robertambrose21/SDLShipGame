#pragma once

class ServerMessagesTransmitter {
public:
    virtual void onClientConnected(int clientIndex) = 0;
    virtual void onClientDisconnected(int clientIndex) = 0;
};