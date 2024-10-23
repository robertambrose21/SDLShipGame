#include "messagelogger.h"

MessageLogger::MessageLogger(const std::string& filename) {
    outfile.open(filename, std::ios::binary | std::ios::trunc);
    outfile.clear();
}

MessageLogger::~MessageLogger() {
    outfile.close();
}

void MessageLogger::printToOutFile(bool isIncoming, int id, const std::string& message) {
    std::string label = isIncoming ? " IN" : "OUT";
    std::time_t t = std::time(nullptr);

    outfile 
        << std::put_time(std::localtime(&t), "[%H:%M:%S] ") 
        << std::format("{} -> {}|{}", label, message, id)
        << std::endl;
}