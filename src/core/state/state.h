#pragma once

class State {
public:
    virtual ~State() {}

    virtual int GetType(void) const = 0;
};