#pragma once

#include <functional>

#include "imgui.h"
#include "itemraritycolours.h"

#include "game/items/equipment.h"
#include "game/weapons/weapon.h"
#include "graphics/graphicscontext.h"
#include "game/application/turncontroller.h"

class InventoryPanel {
private:
    int width;
    int height;
    bool isShown;

    std::function<void(Item* item, Equipment::Slot slot)> onEquipClicked;
    std::function<void(Item* item, Equipment::Slot slot)> onUnequipClicked;
    std::function<void(Item* item)> onEquipWeaponClicked;
    std::function<void(Weapon* weapon)> onUnequipWeaponClicked;
    std::function<void(Item* item)> onExamineClicked;

    void drawEquipment(GraphicsContext& graphicsContext, Participant* participant);
    void drawWeapons(GraphicsContext& graphicsContext, Participant* participant);
    void drawInventoryItem(GraphicsContext& graphicsContext, Item* item);
    void drawEquippedItem(GraphicsContext& graphicsContext, Item* item);
    void drawEquippedWeapon(GraphicsContext& graphicsContext, Weapon* weapon);

public:
    InventoryPanel(int width, int height);

    void draw(GraphicsContext& graphicsContext, Participant* participant);
    void show(void);
    void hide(void);
    void toggle(void);

    void addOnEquipCallback(std::function<void(Item* item, Equipment::Slot slot)>&& callback);
    void addOnUnequipCallback(std::function<void(Item* item, Equipment::Slot slot)>&& callback);
    void addOnEquipWeaponClicked(std::function<void(Item* item)>&& callback);
    void addOnUnequipWeaponClicked(std::function<void(Weapon* weapon)>&& callback);
    void addOnExamineCallback(std::function<void(Item* item)>&& callback);
};