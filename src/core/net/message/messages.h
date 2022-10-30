#pragma once

#include "yojimbo/yojimbo.h"
#include "yojimbo/shared.h"

enum class GameMessageType {
    FIND_PATH,
    SELECT_ENTITY,
    ATTACK_ENTITY,
    COUNT
};

class FindPathMessage : public yojimbo::Message {
public:
    int x, y;

    FindPathMessage() :
        x(0),
        y(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, x, 0, 512);
        serialize_int(stream, y, 0, 512);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class SelectEntityMessage : public yojimbo::Message {
public:
    uint32_t id;

    SelectEntityMessage() :
        id(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_varint32(stream, id);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class AttackEntityMessage : public yojimbo::Message {
public:
    uint32_t entityId;
    uint32_t targetId;
    uint32_t weaponId;

    AttackEntityMessage() :
        entityId(0),
        targetId(0),
        weaponId(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_varint32(stream, entityId);
        serialize_varint32(stream, targetId);
        serialize_varint32(stream, weaponId);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::FIND_PATH, FindPathMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SELECT_ENTITY, SelectEntityMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::ATTACK_ENTITY, AttackEntityMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();