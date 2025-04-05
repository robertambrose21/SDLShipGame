#pragma once

#include "core/net/messagelogger.h"
#include "game/message/messages.h"

class GameMessageLogger : public MessageLogger {
private:
    void logFindPath(FindPathMessage* message, bool isIncoming);
    void logSelectEntity(SelectEntityMessage* message, bool isIncoming);
    void logAttackEntity(AttackMessage* message, bool isIncoming);
    void logGameStateUpdate(GameStateUpdateMessage* message, bool isIncoming);
    void logSetParticipant(SetParticipantMessage* message, bool isIncoming);
    void logSetParticipantAck(SetParticipantAckMessage* message, bool isIncoming);
    void logPassParticipantTurn(PassParticipantTurnMessage* message, bool isIncoming);
    void logLoadMap(LoadMapMessage* message, bool isIncoming);
    void logNextTurn(NextTurnMessage* message, bool isIncoming);
    void logSpawnItems(SpawnItemsMessage* message, bool isIncoming);
    void logTakeItems(TakeItemsMessage* message, bool isIncoming);
    void logEquipItem(EquipItemMessage* message, bool isIncoming);
    void logEquipWeapon(EquipWeaponMessage* message, bool isIncoming);
    void logApplyDamage(ApplyDamageMessage* message, bool isIncoming);
    void logApplyEntityEffect(ApplyEntityEffectMessage* message, bool isIncoming);
    void logApplyGridEffect(ApplyGridEffectMessage* message, bool isIncoming);

public:
    GameMessageLogger(const std::string& filename);

    void logMessage(yojimbo::Message* message, bool isIncoming);
};