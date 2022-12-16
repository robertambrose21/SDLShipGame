#pragma once

class ServerMessagesTransmitter {
public:
    virtual void onClientConnected(const int& clientIndex) = 0;
};