#pragma once

#include <cstdint>

#include "uuid_v4/uuid_v4.h"
#include "uuid_v4/endianness.h"

inline uint32_t getNewId(void) {
    static uint32_t id = 0;
    return id++;
}

struct UUID {
    UUIDv4::UUID uuid;

    UUID()
    { }

    UUID(const std::string& uuidStr) :
        uuid(UUIDv4::UUID::fromStrFactory(uuidStr))
    { }

    std::string getString(void) {
        return uuid.str();
    }

    std::string getBytes(void) {
        return uuid.bytes();
    }

    bool operator==(const UUID& other) const {
        return uuid == other.uuid;
    }

    bool operator<(const UUID& other) const {
        return uuid < other.uuid;
    }

    static UUID fromBytes(const std::string& bytes) {
        UUID uuid;
        uuid.uuid = UUIDv4::UUID(bytes);
        return uuid;
    }

    static inline UUID getNewUUID(void) {
        UUID uuid;
        UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
        uuid.uuid = uuidGenerator.getUUID();
        return uuid;
    }
};
