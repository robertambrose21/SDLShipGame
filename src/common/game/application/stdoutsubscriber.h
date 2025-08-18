#pragma once

#include <iostream>
#include <ctime>
#include <iomanip>
#include <format>
#include <chrono>
#include <string>

#include "spdlog/spdlog.h"

#include "gamecontroller.h"
#include "core/event/eventsubscriber.h"
#include "game/weapons/areaofeffect.h"
#include "game/weapons/projectile.h"

class StdOutSubscriber : 
    public EventSubscriber<ActorEventData>,
    public EventSubscriber<MeleeWeaponEventData>,
    public EventSubscriber<ProjectileEventData>,
    public EventSubscriber<AreaOfEffectEventData>,
    public EventSubscriber<ItemEventData>,
    public EventSubscriber<TakeItemActionEventData>,
    public EventSubscriber<EquipItemActionEventData>,
    public EventSubscriber<ApplyDamageEventData>
{
private:
    std::string getActorIdentifier(Actor* actor);

public:
    StdOutSubscriber();

    void onPublish(const Event<ActorEventData>& event);
    void onPublish(const Event<MeleeWeaponEventData>& event);
    void onPublish(const Event<ProjectileEventData>& event);
    void onPublish(const Event<AreaOfEffectEventData>& event);
    void onPublish(const Event<ItemEventData>& event);
    void onPublish(const Event<TakeItemActionEventData>& event);
    void onPublish(const Event<EquipItemActionEventData>& event);
    void onPublish(const Event<ApplyDamageEventData>& event);
};