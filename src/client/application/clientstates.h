#pragma once

#include "core/state/state.h"

class ClientLoadingState : public State {
public:
    ClientLoadingState();

    int GetType(void) const;
};

class ClientGameLoopState : public State {
public:
    ClientGameLoopState();

    int GetType(void) const;
};