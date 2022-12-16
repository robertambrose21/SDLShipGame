#pragma once

#include <memory>
#include "state.h"

class StateMachine {
protected:
    std::shared_ptr<State> currentState;

public:
    StateMachine();

    void setState(const std::shared_ptr<State>& state);
    std::shared_ptr<State> getCurrentState(void);
};