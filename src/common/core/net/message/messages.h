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
    NEXT_TURN,
    SPAWN_ITEMS,
    TAKE_ITEMS,
    ENGAGEMENT,
    EQUIP_ITEM,
    EQUIP_WEAPON,
    APPLY_DAMAGE,
    COUNT
};

class FindPathMessage : public yojimbo::Message {
public:
    uint32_t entityId;
    
    int x, y;
    int shortStopSteps;
    int turnNumber;

    FindPathMessage() :
        entityId(0),
        x(0),
        y(0),
        shortStopSteps(0),
        turnNumber(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_uint32(stream, entityId);
        serialize_int(stream, x, 0, 512);
        serialize_int(stream, y, 0, 512);
        serialize_int(stream, shortStopSteps, 0, 512);
        serialize_int(stream, turnNumber, 0, 512);
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
        serialize_uint32(stream, id);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class AttackMessage : public yojimbo::Message {
public:
    uint32_t entityId;
    int x, y;
    uint8_t weaponIdBytes[16];
    int turnNumber;

    AttackMessage() :
        entityId(0),
        x(0),
        y(0),
        turnNumber(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_uint32(stream, entityId);
        serialize_int(stream, x, 0, 512);
        serialize_int(stream, y, 0, 512);
        serialize_bytes(stream, weaponIdBytes, 16);
        serialize_int(stream, turnNumber, 0, 512);
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

            serialize_uint32(stream, entity.id);
            serialize_string(stream, entity.name, sizeof(entity.name));
            serialize_bits(stream, entity.participantId, 16);
            serialize_bool(stream, entity.isEngaged);
            serialize_bits(stream, entity.totalHP, 16);
            // TODO: This screws up if currentHP goes negative
            serialize_int(stream, entity.currentHP, -256, 256);
            serialize_bits(stream, entity.x, 16);
            serialize_bits(stream, entity.y, 16);

            // Weapons
            serialize_bytes(stream, entity.currentWeaponIdBytes, 16);
            serialize_int(stream, entity.numWeapons, 0, MaxWeapons);
            for(int j = 0; j < entity.numWeapons; j++) {
                auto& weapon = entity.weaponUpdates[j];
                
                serialize_bytes(stream, weapon.idBytes, 16);
                serialize_string(stream, weapon.name, sizeof(weapon.name));
                serialize_string(stream, weapon.weaponClass, sizeof(weapon.weaponClass));
                serialize_string(stream, weapon.projectile, sizeof(weapon.projectile));
                serialize_bits(stream, weapon.damage, 16);
                serialize_bits(stream, weapon.range, 16);
                serialize_bits(stream, weapon.uses, 16);
                serialize_bits(stream, weapon.usesLeft, 16);
                serialize_bool(stream, weapon.hasItem);
                serialize_uint32(stream, weapon.itemId);
            }
        }
        
        serialize_int(stream, gameStateUpdate.currentParticipantId, 0, 64);

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
        serialize_uint32(stream, data);
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
        serialize_int(stream, mapBlock.numSequences, 0, 256);
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
    int numDice;
    DiceActionResult dice[64];

    ActionsRollResponseMessage() :
        participantId(0),
        numDice(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, participantId, 0, 64);
        serialize_int(stream, numDice, 1, 64);
        for(int i = 0; i < numDice; i++) {
            serialize_int(stream, dice[i].rollNumber, 1, 6);

            for(int j = 0; j < 6; j++) {
                serialize_int(stream, dice[i].actions[j], 0, 255);
            }
        }

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class NextTurnMessage : public yojimbo::Message {
public:
    int participantId;
    int turnNumber;

    NextTurnMessage() :
        participantId(0),
        turnNumber(0) 
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, participantId, 0, 64);
        serialize_int(stream, turnNumber, 0, UINT16_MAX); // Max turn number??

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class SpawnItemsMessage : public yojimbo::Message {
public:
    int x, y;
    uint32_t ownerId;
    int numItems;
    ItemUpdate items[64];

    SpawnItemsMessage() :
        x(0),
        y(0),
        ownerId(0),
        numItems(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_bits(stream, x, 16);
        serialize_bits(stream, y, 16);
        serialize_uint32(stream, ownerId);
        serialize_int(stream, numItems, 0, 64);
        
        for(int i = 0; i < numItems; i++) {
            serialize_uint32(stream, items[i].id);
            serialize_string(stream, items[i].name, sizeof(items[i].name));
        }

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class TakeItemsMessage : public yojimbo::Message {
public:
    int turnNumber;
    uint32_t entityId;
    int numItems;
    ItemUpdate items[64];

    TakeItemsMessage() :
        turnNumber(0),
        entityId(0),
        numItems(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, turnNumber, 0, 512);
        serialize_uint32(stream, entityId);
        serialize_int(stream, numItems, 0, 64);
        
        for(int i = 0; i < numItems; i++) {
            serialize_uint32(stream, items[i].id);
            serialize_string(stream, items[i].name, sizeof(items[i].name));
        }

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class EngagementMessage : public yojimbo::Message {
public:
    int participantIdA;
    int participantIdB;
    int type;

    EngagementMessage() :
        participantIdA(0),
        participantIdB(0),
        type(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, participantIdA, 0, 64);
        serialize_int(stream, participantIdB, 0, 64);
        serialize_int(stream, type, 0, 64);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class EquipItemMessage : public yojimbo::Message {
public:
    uint32_t itemId;
    uint32_t entityId;
    uint8_t slot;
    bool isUnequip;

    EquipItemMessage() :
        itemId(0),
        entityId(0),
        slot(0),
        isUnequip(false)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_uint32(stream, itemId);
        serialize_uint32(stream, entityId);
        serialize_bits(stream, slot, 8);
        serialize_bool(stream, isUnequip);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class EquipWeaponMessage : public yojimbo::Message {
public:
    uint32_t itemId;
    uint32_t entityId;
    uint8_t weaponIdBytes[16];
    bool isUnequip;

    EquipWeaponMessage() :
        itemId(0),
        entityId(0),
        isUnequip(false)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_uint32(stream, itemId);
        serialize_uint32(stream, entityId);
        serialize_bytes(stream, weaponIdBytes, 16);
        serialize_bool(stream, isUnequip);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class ApplyDamageMessage : public yojimbo::Message {
public:
    int fromId;
    uint32_t targetId;
    uint8_t source;
    int damage;

    ApplyDamageMessage() :
        fromId(0),
        targetId(0),
        source(0),
        damage(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, fromId, 0, 64);
        serialize_uint32(stream, targetId);
        serialize_bits(stream, source, 8);
        serialize_int(stream, damage, 0, 4096);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::FIND_PATH, FindPathMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SELECT_ENTITY, SelectEntityMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::ATTACK_ENTITY, AttackMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::GAME_STATE_UPDATE, GameStateUpdateMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::TEST_MESSAGE, GameTestMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SET_PARTICIPANT, SetParticipantMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SET_PARTICIPANT_ACK, SetParticipantAckMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::PASS_PARTICIPANT_TURN, PassParticipantTurnMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::LOAD_MAP, LoadMapMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::ACTIONS_ROLL, ActionsRollMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::ACTIONS_ROLL_RESPONSE, ActionsRollResponseMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::NEXT_TURN, NextTurnMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SPAWN_ITEMS, SpawnItemsMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::TAKE_ITEMS, TakeItemsMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::ENGAGEMENT, EngagementMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::EQUIP_ITEM, EquipItemMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::EQUIP_WEAPON, EquipWeaponMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::APPLY_DAMAGE, ApplyDamageMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();