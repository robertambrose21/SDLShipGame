#pragma once

#include <functional>

#include "itemraritycolours.h"

#include "game/actors/actor.h"
#include "graphics/graphicscontext.h"
#include "core/event/eventsubscriber.h"

class ActorPanel : public EventSubscriber<ActorUpdateStatsEventData>{
private:
    int width;
    int height;
    bool isOpen;

    Actor* actor;
    std::map<Stats::StatCategory, std::vector<Stats::StatsPair>> stats;

    std::function<void(Item* item, Equippable<Stats::GearStats>::Slot slot)> onUnequipClicked;
    std::function<void(Weapon* weapon)> onUnequipWeaponClicked;
    std::function<void(Item* item)> onExamineClicked;

    void drawStats(GraphicsContext& graphicsContext);
    void drawStat(const std::string& name, const std::string& value);
    void drawEquipment(GraphicsContext& graphicsContext);
    void drawWeapons(GraphicsContext& graphicsContext);
    void drawEquippedItem(GraphicsContext& graphicsContext, Item* item);
    void drawEquippedWeapon(GraphicsContext& graphicsContext, Weapon* weapon);

public:
    ActorPanel(int width, int height, Actor* actor);
    ActorPanel() = default;
    ActorPanel(const ActorPanel&) = delete;
    ActorPanel& operator=(const ActorPanel&) = delete;

    void draw(GraphicsContext& graphicsContext);

    void addOnUnequipCallback(std::function<void(Item* item, Equippable<Stats::GearStats>::Slot slot)>&& callback);
    void addOnUnequipWeaponCallback(std::function<void(Weapon* weapon)>&& callback);
    void addOnExamineCallback(std::function<void(Item* item)>&& callback);

    bool getIsOpen(void) const;

    void onPublish(const Event<ActorUpdateStatsEventData>& event);
};
