#include "clientstates.h"
#include "clientstatemachine.h"

ClientLoadingState::ClientLoadingState()
{ }

int ClientLoadingState::GetType(void) const {
    return ClientStateMachine::Loading;
}

ClientGameLoopState::ClientGameLoopState()
{ }

int ClientGameLoopState::GetType(void) const {
    return ClientStateMachine::GameLoop;
}