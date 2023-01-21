#pragma once

#include "core/net/yojimboimport.h"
#include "game/net/messages.h"

// TODO: Move this file to game project
// Note: Yojimbo seems to have a limit on the size of the messages which can be sent
// be careful about how big some of these messages are
enum class GameMessageType {
    FIND_PATH,
    SELECT_ENTITY,
    ATTACK_ENTITY,
    GAME_STATE_UPDATE,
    TEST_MESSAGE,
    SET_PARTICIPANT,
    SET_PARTICIPANT_ACK,
    PASS_PARTICIPANT_TURN,
    LOAD_MAP,
    ACTIONS_ROLL,
    ACTIONS_ROLL_RESPONSE,
    COUNT
};

class FindPathMessage : public yojimbo::Message {
public:
    uint32_t entityId;
    
    int x, y;
    int shortStopSteps;

    FindPathMessage() :
        entityId(0),
        x(0),
        y(0),
        shortStopSteps(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_varint32(stream, entityId);
        serialize_int(stream, x, 0, 512);
        serialize_int(stream, y, 0, 512);
        serialize_int(stream, shortStopSteps, 0, 512);
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

// TODO: Serialize functions for entities/weapons
class GameStateUpdateMessage : public yojimbo::Message {
public:
    GameStateUpdate gameStateUpdate;

    GameStateUpdateMessage()
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, gameStateUpdate.numEntities, 0, MaxEntities);
        for(int i = 0; i < gameStateUpdate.numEntities; i++) {
            auto& entity = gameStateUpdate.entities[i];

            serialize_varint32(stream, entity.id);
            serialize_string(stream, entity.name, sizeof(entity.name));
            serialize_bits(stream, entity.participantId, 16);
            serialize_bits(stream, entity.totalHP, 16);
            // TODO: This screws up if currentHP goes negative
            serialize_int(stream, entity.currentHP, -256, 256);
            serialize_bits(stream, entity.x, 16);
            serialize_bits(stream, entity.y, 16);
            serialize_bits(stream, entity.movesPerTurn, 16);
            serialize_bits(stream, entity.movesLeft, 16);

            // Weapons
            serialize_varint32(stream, entity.currentWeaponId);
            serialize_int(stream, entity.numWeapons, 0, MaxWeapons);
            for(int j = 0; j < entity.numWeapons; j++) {
                auto& weapon = entity.weaponUpdates[j];

                serialize_varint32(stream, weapon.id);
                serialize_string(stream, weapon.name, sizeof(weapon.name));
                serialize_string(stream, weapon.weaponClass, sizeof(weapon.weaponClass));
                serialize_string(stream, weapon.projectile, sizeof(weapon.projectile));
                serialize_bits(stream, weapon.damage, 16);
                serialize_bits(stream, weapon.range, 16);
                serialize_bits(stream, weapon.uses, 16);
                serialize_bits(stream, weapon.usesLeft, 16);
            }
        }
        
        serialize_int(stream, gameStateUpdate.currentParticipant, 0, 64);

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
    int numParticipantsToSet;
    bool isPlayer;

    SetParticipantMessage() :
        participantId(0),
        numParticipantsToSet(0),
        isPlayer(false)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, participantId, 0, 64);
        serialize_int(stream, numParticipantsToSet, 0, 64);
        serialize_bool(stream, isPlayer);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class SetParticipantAckMessage : public yojimbo::Message {
public:
    int participantId;

    SetParticipantAckMessage() :
        participantId(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, participantId, 0, 64);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class PassParticipantTurnMessage : public yojimbo::Message {
public:
    int participantId;

    PassParticipantTurnMessage() :
        participantId(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, participantId, 0, 64);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class LoadMapMessage : public yojimbo::Message {
public:
    MapBlock mapBlock;

    LoadMapMessage()
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, mapBlock.width, 0, 256);
        serialize_int(stream, mapBlock.height, 0, 256);
        serialize_int(stream, mapBlock.sequence, 0, 256);
        serialize_int(stream, mapBlock.totalSize, 0, UINT16_MAX);
        serialize_int(stream, mapBlock.blockSize, 0, MaxMapBlockSize);

        for(int i = 0; i < mapBlock.blockSize; i++) {
            serialize_bits(stream, mapBlock.data[i], 8);
        }
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class ActionsRollMessage : public yojimbo::Message {
public:
    int participantId;

    ActionsRollMessage() :
        participantId(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, participantId, 0, 64);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class ActionsRollResponseMessage : public yojimbo::Message {
public:
    int participantId;
    int rollNumber;
    int actions[6] = { 0 }; 

    ActionsRollResponseMessage() :
        participantId(0),
        rollNumber(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, participantId, 0, 64);
        serialize_int(stream, rollNumber, 1, 6);
        for(int i = 0; i < rollNumber; i++) {
            serialize_int(stream, actions[i], 0, 255);
        }

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
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SET_PARTICIPANT_ACK, SetParticipantAckMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::PASS_PARTICIPANT_TURN, PassParticipantTurnMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::LOAD_MAP, LoadMapMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::ACTIONS_ROLL, ActionsRollMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::ACTIONS_ROLL_RESPONSE, ActionsRollResponseMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();