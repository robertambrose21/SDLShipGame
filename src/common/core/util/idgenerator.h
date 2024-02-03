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

    UUID(const UUIDv4::UUID uuid) :
        uuid(uuid)
    { }

    UUID(const std::string& uuidStr) :
        uuid(UUIDv4::UUID::fromStrFactory(uuidStr))
    { }

    std::string getString(void) {
        return uuid.str();
    }

    bool operator==(const UUID& other) const {
        return uuid == other.uuid;
    }

    static inline UUID getNewUUID(void) {
        UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
        UUIDv4::UUID uuid = uuidGenerator.getUUID();
        return UUID(uuid);
    }
};
