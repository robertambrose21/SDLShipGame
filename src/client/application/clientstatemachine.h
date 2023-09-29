#pragma once

#include "core/state/statemachine.h"

class ClientStateMachine : public StateMachine {
public:
    enum : int {
        Loading,
        GameLoop
    };

    ClientStateMachine();
};