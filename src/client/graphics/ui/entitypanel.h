#pragma once

#include <functional>

#include "itemraritycolours.h"

#include "game/entities/entity.h"
#include "graphics/graphicscontext.h"
#include "core/event/eventsubscriber.h"

class EntityPanel : public EventSubscriber<EntityUpdateStatsEventData>{
private:
    int width;
    int height;
    bool isOpen;

    Entity* entity;
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
    EntityPanel(int width, int height, Entity* entity);
    EntityPanel() = default;
    EntityPanel(const EntityPanel&) = delete;
    EntityPanel& operator=(const EntityPanel&) = delete;

    void draw(GraphicsContext& graphicsContext);

    void addOnUnequipCallback(std::function<void(Item* item, Equippable<Stats::GearStats>::Slot slot)>&& callback);
    void addOnUnequipWeaponClicked(std::function<void(Weapon* weapon)>&& callback);
    void addOnExamineCallback(std::function<void(Item* item)>&& callback);

    bool getIsOpen(void) const;

    void onPublish(const Event<EntityUpdateStatsEventData>& event);
};
