#pragma once

#include <iostream>
#include <ctime>
#include <iomanip>
#include <format>
#include <chrono>
#include <string>

#include "turncontroller.h"
#include "core/event/eventsubscriber.h"
#include "game/weapons/areaofeffect.h"
#include "game/weapons/projectile.h"

class StdOutSubscriber : 
    public EventSubscriber<TurnEventData>,
    public EventSubscriber<EntityEventData>,
    public EventSubscriber<WeaponEventData>,
    public EventSubscriber<ProjectileEventData>,
    public EventSubscriber<AreaOfEffectEventData>,
    public EventSubscriber<ItemEventData>,
    public EventSubscriber<TakeItemActionEventData>,
    public EventSubscriber<EngagementEventData>,
    public EventSubscriber<EquipItemActionEventData>,
    public EventSubscriber<EquipWeaponActionEventData>
{
private:
    std::string getEntityIdentifier(Entity* entity);

public:
    StdOutSubscriber();

    void onPublish(const Event<TurnEventData>& event);
    void onPublish(const Event<EntityEventData>& event);
    void onPublish(const Event<WeaponEventData>& event);
    void onPublish(const Event<ProjectileEventData>& event);
    void onPublish(const Event<AreaOfEffectEventData>& event);
    void onPublish(const Event<ItemEventData>& event);
    void onPublish(const Event<TakeItemActionEventData>& event);
    void onPublish(const Event<EngagementEventData>& event);
    void onPublish(const Event<EquipItemActionEventData>& event);
    void onPublish(const Event<EquipWeaponActionEventData>& event);
};