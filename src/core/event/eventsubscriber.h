#pragma once

#include "event.h"

template<typename T>
class EventSubscriber {
public:
    virtual void onPublish(const Event<T>& event) = 0;
};