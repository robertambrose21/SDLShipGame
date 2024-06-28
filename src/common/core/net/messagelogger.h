#pragma once

#include <string>
#include <fstream>
#include <format>
#include <ctime>
#include <iomanip>

#include "core/net/yojimboimport.h"

class MessageLogger {
private:
    std::ofstream outfile;

public:
    MessageLogger(const std::string& filename);
    ~MessageLogger();

    void printToOutFile(bool isIncoming, int id, const std::string& message);
    virtual void logMessage(yojimbo::Message* message, bool isIncoming) = 0;
};