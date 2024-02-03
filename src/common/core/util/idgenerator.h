#pragma once

#include <cstdint>

#include "uuid_v4/uuid_v4.h"
#include "uuid_v4/endianness.h"

inline uint32_t getNewId(void) {
    static uint32_t id = 0;
    return id++;
}

inline std::string getNewUUID(void) {
    UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
    UUIDv4::UUID uuid = uuidGenerator.getUUID();
    return uuid.str();
}