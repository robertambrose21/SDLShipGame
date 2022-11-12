#pragma once

#include "yojimbo/yojimbo.h"
#include "yojimbo/shared.h"
#include "game/net/gamestateupdate.h"

// TODO: Move this file to game project

enum class GameMessageType {
    FIND_PATH,
    SELECT_ENTITY,
    ATTACK_ENTITY,
    GAME_STATE_UPDATE,
    TEST_MESSAGE,
    SET_PARTICIPANT,
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

class GameStateUpdateMessage : public yojimbo::Message {
public:
    GameStateUpdate gameStateUpdate;

    GameStateUpdateMessage()
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, gameStateUpdate.numEntities, 0, MaxEntities);
        for(int i = 0; i < gameStateUpdate.numEntities; i++) {
            serialize_varint32(stream, gameStateUpdate.entities[i].id);
            serialize_bits(stream, gameStateUpdate.entities[i].currentHP, 8);
            serialize_bits(stream, gameStateUpdate.entities[i].x, 8);
            serialize_bits(stream, gameStateUpdate.entities[i].y, 8);
        }

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class GameTestMessage : public yojimbo::Message {
public:
    uint32_t data;

    GameTestMessage()
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_varint32(stream, data);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class SetParticipantMessage : public yojimbo::Message {
public:
    int participantId;

    SetParticipantMessage() :
        participantId(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, participantId, 0, 64);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::FIND_PATH, FindPathMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SELECT_ENTITY, SelectEntityMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::ATTACK_ENTITY, AttackEntityMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::GAME_STATE_UPDATE, GameStateUpdateMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::TEST_MESSAGE, GameTestMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SET_PARTICIPANT, SetParticipantMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();