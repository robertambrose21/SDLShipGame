#include "statemachine.h"

StateMachine::StateMachine()
{ }

void StateMachine::setState(std::unique_ptr<State> state) {
    currentState = std::move(state);
}

State* StateMachine::getCurrentState(void) {
    return currentState.get();
}