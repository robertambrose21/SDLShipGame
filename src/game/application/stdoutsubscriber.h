#pragma once

#include <iostream>

#include "turncontroller.h"
#include "core/event/eventsubscriber.h"

class StdOutSubscriber : public EventSubscriber<TurnControllerEventData> {
public:
    StdOutSubscriber();
    void onPublish(const Event<TurnControllerEventData>& event);
};