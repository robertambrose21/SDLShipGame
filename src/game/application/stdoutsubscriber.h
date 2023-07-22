#pragma once

#include <iostream>

#include "turncontroller.h"
#include "core/event/eventsubscriber.h"
#include "game/weapons/areaofeffect.h"

class StdOutSubscriber : 
    public EventSubscriber<TurnControllerEventData>,
    public EventSubscriber<EntityEventData>,
    public EventSubscriber<WeaponEventData>,
    public EventSubscriber<ProjectileEventData>,
    public EventSubscriber<AreaOfEffectEventData>
{
private:
    std::string getEntityIdentifier(Entity* entity);

public:
    StdOutSubscriber();

    void onPublish(const Event<TurnControllerEventData>& event);
    void onPublish(const Event<EntityEventData>& event);
    void onPublish(const Event<WeaponEventData>& event);
    void onPublish(const Event<ProjectileEventData>& event);
    void onPublish(const Event<AreaOfEffectEventData>& event);
};