#pragma once

#include <cstdint>
#include <span>

#include "stduuid/include/uuid.h"

inline uint32_t getNewId(void) {
    static uint32_t id = 0;
    return id++;
}

struct UUID {
    uuids::uuid uuid;

    UUID()
    { }

    UUID(const std::string& uuidStr) {
        auto id = uuids::uuid::from_string(uuidStr);

        if(!id.has_value()) {
            throw std::runtime_error("Invalid UUID string: " + uuidStr);
        }

        uuid = id.value();
    }

    std::string getString(void) {
        return uuids::to_string(uuid);
    }

    std::span<std::byte const, 16> getBytes(void) {
        return uuid.as_bytes();
    }

    bool operator==(const UUID& other) const {
        return uuid == other.uuid;
    }

    bool operator<(const UUID& other) const {
        return uuid < other.uuid;
    }

    static UUID fromBytes(std::span<uint8_t, 16> bytes) {
        UUID uuid;
        uuid.uuid = uuids::uuid(bytes);
        return uuid;
    }

    static UUID fromBytes(const uint8_t bytes[16]) {
        UUID uuid;
        uint8_t data[16];
        memcpy(data, bytes, 16);
        uuid.uuid = uuids::uuid(data);
        return uuid;
    }

    static inline UUID getNewUUID(void) {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size> {};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 generator(seq);
        uuids::uuid_random_generator gen{generator};

        UUID uuid;
        uuid.uuid = gen();
        return uuid;
    }
};

