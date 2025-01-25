#pragma once

#include "core/net/yojimboimport.h"
#include "game/net/messages.h"

// Note: Yojimbo seems to have a limit on the size of the messages which can be sent
// be careful about how big some of these messages are
enum class GameMessageType {
    PING,
    FIND_PATH,
    SELECT_ENTITY,
    ATTACK_ENTITY,
    GAME_STATE_UPDATE,
    TEST_MESSAGE,
    SET_PARTICIPANT,
    SET_PARTICIPANT_ACK,
    PASS_PARTICIPANT_TURN,
    LOAD_MAP,
    ACTIONS_ROLL, // remove
    ACTIONS_ROLL_RESPONSE, // remove
    NEXT_TURN,
    SPAWN_ITEMS,
    TAKE_ITEMS,
    ENGAGEMENT,
    EQUIP_ITEM,
    EQUIP_WEAPON,
    APPLY_DAMAGE,
    APPLY_ENTITY_EFFECT,
    APPLY_GRID_EFFECT,
    TILES_REVEALED,
    SET_ENTITY_POSITION,
    REMOVE_ENTITY_VISIBILITY,
    ADD_ENTITY_VISIBILITY,
    SET_TURN,
    COUNT
};

// TODO: Remove these and use serialize_int
#define serialize_uint32(stream, value) serialize_int(stream, value, 0, INT32_MAX)
#define serialize_uint64(stream, value) serialize_int(stream, value, 0, INT32_MAX)

class PingMessage : public yojimbo::Message {
public:
    PingMessage()
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
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

template <typename Stream>
bool serialize_weapon_state_update(Stream& stream, WeaponStateUpdate& weaponStateUpdate) {
    serialize_bytes(stream, weaponStateUpdate.idBytes, 16);
    serialize_string(stream, weaponStateUpdate.name, sizeof(weaponStateUpdate.name));
    serialize_string(stream, weaponStateUpdate.weaponClass, sizeof(weaponStateUpdate.weaponClass));
    serialize_string(stream, weaponStateUpdate.projectile, sizeof(weaponStateUpdate.projectile));
    serialize_bits(stream, weaponStateUpdate.damage, 16);
    serialize_bits(stream, weaponStateUpdate.range, 16);
    serialize_bits(stream, weaponStateUpdate.uses, 16);
    serialize_bits(stream, weaponStateUpdate.usesLeft, 16);
    serialize_bool(stream, weaponStateUpdate.hasItem);
    serialize_uint32(stream, weaponStateUpdate.itemId);
    
    return true;
}

template <typename Stream>
bool serialize_entity_state_update(Stream& stream, EntityStateUpdate& entityStateUpdate) {
    serialize_uint32(stream, entityStateUpdate.id);
    serialize_string(stream, entityStateUpdate.name, sizeof(entityStateUpdate.name));
    serialize_bits(stream, entityStateUpdate.participantId, 16);
    serialize_bool(stream, entityStateUpdate.isEngaged);
    serialize_bits(stream, entityStateUpdate.totalHP, 16);
    // TODO: This screws up if currentHP goes negative
    serialize_int(stream, entityStateUpdate.currentHP, -256, 256);
    serialize_bits(stream, entityStateUpdate.x, 16);
    serialize_bits(stream, entityStateUpdate.y, 16);

    // Weapons
    serialize_bytes(stream, entityStateUpdate.currentWeaponIdBytes, 16);
    serialize_int(stream, entityStateUpdate.numWeapons, 0, MaxWeapons);
    for(int j = 0; j < entityStateUpdate.numWeapons; j++) {
        serialize_weapon_state_update(stream, entityStateUpdate.weaponUpdates[j]);
    }
    
    return true;
}

class GameStateUpdateMessage : public yojimbo::Message {
public:
    GameStateUpdate gameStateUpdate;

    GameStateUpdateMessage()
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, gameStateUpdate.numEntities, 0, MaxEntities);
        for(int i = 0; i < gameStateUpdate.numEntities; i++) {
            serialize_entity_state_update(stream, gameStateUpdate.entities[i]);
        }
        
        serialize_int(stream, gameStateUpdate.currentParticipantId, 0, 64);
        serialize_bits(stream, gameStateUpdate.numExpectedChunks, 8);
        serialize_uint32(stream, gameStateUpdate.chunkId);

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
    uint64_t clientId;

    SetParticipantMessage() :
        participantId(0),
        numParticipantsToSet(0),
        clientId(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, participantId, 0, 64);
        serialize_int(stream, numParticipantsToSet, 0, 64);
        serialize_uint64(stream, clientId);

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
    char droppedBy[64];
    int numItems;
    ItemUpdate items[64];

    SpawnItemsMessage() :
        x(0),
        y(0),
        numItems(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_bits(stream, x, 16);
        serialize_bits(stream, y, 16);
        serialize_string(stream, droppedBy, sizeof(droppedBy));
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
    int turnToEngageOn;

    EngagementMessage() :
        participantIdA(0),
        participantIdB(0),
        type(0),
        turnToEngageOn(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, participantIdA, 0, 64);
        serialize_int(stream, participantIdB, 0, 64);
        serialize_int(stream, type, 0, 64);
        serialize_int(stream, turnToEngageOn, 0, 512);
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

class ApplyEntityEffectMessage : public yojimbo::Message {
public:
    uint8_t type;
    uint32_t targetId;
    EffectStatsUpdate effectStats;

    ApplyEntityEffectMessage() :
        type(0),
        targetId(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_bits(stream, type, 8);
        serialize_uint32(stream, targetId);
        serialize_bits(stream, effectStats.effectType, 8);
        serialize_bits(stream, effectStats.duration, 8);
        serialize_bits(stream, effectStats.numDamageTicks, 8);

        for(int i = 0; i < effectStats.numDamageTicks; i++) {
            serialize_uint32(stream, effectStats.damageTicks[i]);
        }

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class ApplyGridEffectMessage : public yojimbo::Message {
public:
    uint8_t type;
    int x, y;
    uint8_t duration;

    ApplyGridEffectMessage() :
        type(0),
        x(0),
        y(0),
        duration(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_bits(stream, type, 8);
        serialize_int(stream, x, 0, 512);
        serialize_int(stream, y, 0, 512);
        serialize_bits(stream, duration, 8);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class TilesRevealedMessage : public yojimbo::Message {
public:
    // TODO: Common item
    struct Tile {
        uint16_t id;
        uint8_t orientation;
        uint16_t x, y;
        // is frozen needed?
    };

    uint8_t participantId;
    uint8_t numRevealedTiles;
    Tile revealedTiles[64];

    TilesRevealedMessage() :
        participantId(0),
        numRevealedTiles(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_bits(stream, participantId, 8);
        serialize_bits(stream, numRevealedTiles, 8);

        for(int i = 0; i < numRevealedTiles; i++) {
            serialize_bits(stream, revealedTiles[i].id, 16);
            serialize_bits(stream, revealedTiles[i].orientation, 8);
            serialize_bits(stream, revealedTiles[i].x, 16);
            serialize_bits(stream, revealedTiles[i].y, 16);
        }

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class SetEntityPositionMessage : public yojimbo::Message {
public:
    uint32_t entityId;
    uint8_t movesLeft;
    uint16_t x, y;

    SetEntityPositionMessage() :
        entityId(0),
        x(0),
        y(0)
    { }

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_uint32(stream, entityId);
        serialize_bits(stream, movesLeft, 8);
        serialize_bits(stream, x, 16);
        serialize_bits(stream, y, 16);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class RemoveEntityVisibilityMessage : public yojimbo::Message {
public:
    uint32_t entityId;
    uint8_t visibleToParticipantId;

    RemoveEntityVisibilityMessage() :
        entityId(0),
        visibleToParticipantId(0)
    { }

    template<typename Stream>
    bool Serialize(Stream& stream) {
        serialize_uint32(stream, entityId);
        serialize_bits(stream, visibleToParticipantId, 8);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class AddEntityVisibilityMessage : public yojimbo::Message {
public:
    EntityStateUpdate entity;
    uint8_t visibleToParticipantId;

    AddEntityVisibilityMessage() :
        visibleToParticipantId(0)
    { }

    template<typename Stream>
    bool Serialize(Stream& stream) {
        serialize_entity_state_update(stream, entity);
        serialize_bits(stream, visibleToParticipantId, 8);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class SetTurnMessage : public yojimbo::Message {
public:
    uint32_t turnNumber;
    uint8_t currentParticipantId;

    SetTurnMessage() :
        turnNumber(0),
        currentParticipantId(0)
    { }

    template<typename Stream>
    bool Serialize(Stream& stream) {
        serialize_uint32(stream, turnNumber);
        serialize_bits(stream, currentParticipantId, 8);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::PING, PingMessage);
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
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::APPLY_ENTITY_EFFECT, ApplyEntityEffectMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::APPLY_GRID_EFFECT, ApplyGridEffectMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::TILES_REVEALED, TilesRevealedMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SET_ENTITY_POSITION, SetEntityPositionMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::REMOVE_ENTITY_VISIBILITY, RemoveEntityVisibilityMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::ADD_ENTITY_VISIBILITY, AddEntityVisibilityMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SET_TURN, SetTurnMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();