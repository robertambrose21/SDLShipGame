#include "gamemessagelogger.h"

GameMessageLogger::GameMessageLogger(const std::string& filename) :
    MessageLogger(filename)
{ }

void GameMessageLogger::logMessage(yojimbo::Message* message, bool isIncoming) {
    switch(message->GetType()) {
        case (int) GameMessageType::GAME_STATE_UPDATE:      { logGameStateUpdate((GameStateUpdateMessage*) message, isIncoming); break; }
        case (int) GameMessageType::SET_PARTICIPANT:        { logSetParticipant((SetParticipantMessage*) message, isIncoming); break; }
        case (int) GameMessageType::LOAD_MAP:               { logLoadMap((LoadMapMessage*) message, isIncoming); break; }
        case (int) GameMessageType::FIND_PATH:              { logFindPath((FindPathMessage*) message, isIncoming); break; }
        case (int) GameMessageType::ATTACK_ENTITY:          { logAttackEntity((AttackMessage*) message, isIncoming); break; }
        case (int) GameMessageType::NEXT_TURN:              { logNextTurn((NextTurnMessage*) message, isIncoming); break; }
        case (int) GameMessageType::SPAWN_ITEMS:            { logSpawnItems((SpawnItemsMessage*) message, isIncoming); break; }
        case (int) GameMessageType::TAKE_ITEMS:             { logTakeItems((TakeItemsMessage*) message, isIncoming); break; }
        case (int) GameMessageType::ENGAGEMENT:             { logEngagement((EngagementMessage*) message, isIncoming); break; }
        case (int) GameMessageType::APPLY_DAMAGE:           { logApplyDamage((ApplyDamageMessage*) message, isIncoming); break; }
        case (int) GameMessageType::APPLY_ENTITY_EFFECT:    { logApplyEntityEffect((ApplyEntityEffectMessage*) message, isIncoming); break; }
        case (int) GameMessageType::APPLY_GRID_EFFECT:      { logApplyGridEffect((ApplyGridEffectMessage*) message, isIncoming); break; }
        case (int) GameMessageType::SET_PARTICIPANT_ACK:    { logSetParticipantAck((SetParticipantAckMessage*) message, isIncoming); break; }
        case (int) GameMessageType::PASS_PARTICIPANT_TURN:  { logPassParticipantTurn((PassParticipantTurnMessage*) message, isIncoming); break; }
        case (int) GameMessageType::EQUIP_ITEM:             { logEquipItem((EquipItemMessage*) message, isIncoming); break; }
        case (int) GameMessageType::EQUIP_WEAPON:           { logEquipWeapon((EquipWeaponMessage*) message, isIncoming); break; }
        case (int) GameMessageType::SELECT_ENTITY:          { logSelectEntity((SelectEntityMessage*) message, isIncoming); break; }

        default:
            printToOutFile(isIncoming, message->GetId(), "Processed unknown message");
            break;
    }
}

void GameMessageLogger::logFindPath(FindPathMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "FindPathMessage");
}

void GameMessageLogger::logSelectEntity(SelectEntityMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "SelectEntityMessage");
}

void GameMessageLogger::logAttackEntity(AttackMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "AttackMessage");
}

void GameMessageLogger::logGameStateUpdate(GameStateUpdateMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "GameStateUpdateMessage");
}

void GameMessageLogger::logSetParticipant(SetParticipantMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "SetParticipantMessage");
}

void GameMessageLogger::logSetParticipantAck(SetParticipantAckMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "SetParticipantAckMessage");
}

void GameMessageLogger::logPassParticipantTurn(PassParticipantTurnMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "PassParticipantTurnMessage");
}

void GameMessageLogger::logLoadMap(LoadMapMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "LoadMapMessage");
}

void GameMessageLogger::logNextTurn(NextTurnMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "NextTurnMessage");
}

void GameMessageLogger::logSpawnItems(SpawnItemsMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "SpawnItemsMessage");
}

void GameMessageLogger::logTakeItems(TakeItemsMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "TakeItemsMessage");
}

void GameMessageLogger::logEngagement(EngagementMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "EngagementMessage");
}

void GameMessageLogger::logEquipItem(EquipItemMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "EquipItemMessage");
}

void GameMessageLogger::logEquipWeapon(EquipWeaponMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "EquipWeaponMessage");
}

void GameMessageLogger::logApplyDamage(ApplyDamageMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "ApplyDamageMessage");
}

void GameMessageLogger::logApplyEntityEffect(ApplyEntityEffectMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "ApplyEntityEffectMessage");
}

void GameMessageLogger::logApplyGridEffect(ApplyGridEffectMessage* message, bool isIncoming) {
    printToOutFile(isIncoming, message->GetId(), "ApplyGridEffectMessage");
}