#pragma once

#include <memory>
#include "state.h"

class StateMachine {
protected:
    std::unique_ptr<State> currentState;

public:
    StateMachine();

    void setState(std::unique_ptr<State> state);
    State* getCurrentState(void);
};