#include "statemachine.h"

StateMachine::StateMachine()
{ }

void StateMachine::setState(std::shared_ptr<State> state) {
    currentState = state;
}

std::shared_ptr<State> StateMachine::getCurrentState(void) {
    return currentState;
}