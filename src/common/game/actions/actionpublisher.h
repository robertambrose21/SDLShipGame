#pragma once

#include <tuple>

#include "game/event/events.h"
#include "core/event/eventpublisher.h"

class ActionPublisher : 
    public EventPublisher<
        MoveActionEventData,
        AttackActionEventData,
        TakeItemActionEventData,
        EngagementEventData,
        EquipItemActionEventData,
        EquipWeaponActionEventData
    >
{ };
