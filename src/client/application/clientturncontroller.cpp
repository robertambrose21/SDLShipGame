#include "clientturncontroller.h"

ClientTurnController::ClientTurnController() :
    TurnController(),
    receivedValidNextTurnFlag(false)
{ }

bool ClientTurnController::canProgressToNextTurn(int participantId) {
    if(receivedValidNextTurnFlag) {
        receivedValidNextTurnFlag = false;
        return true;
    }

    return false;
}

void ClientTurnController::receiveSetNextTurnFlag(int participantId, int turnNumber) {
    if(this->currentParticipant == participantId && this->turnNumber == turnNumber) {
        receivedValidNextTurnFlag = true;
    }
    // TODO: else, handle desync?
}