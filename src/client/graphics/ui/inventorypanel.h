#pragma once

#include <functional>

#include "imgui.h"
#include "itemraritycolours.h"

#include "game/items/equippable.h"
#include "game/weapons/weapon.h"
#include "graphics/graphicscontext.h"
#include "game/application/turncontroller.h"

class InventoryPanel {
private:
    int width;
    int height;
    bool isShown;

    std::function<void(Item* item, Equippable<Stats::GearStats>::Slot slot)> onEquipClicked;
    std::function<void(Item* item)> onEquipWeaponClicked;
    std::function<void(Item* item)> onExamineClicked;

    void drawInventoryItem(GraphicsContext& graphicsContext, Item* item);

public:
    InventoryPanel(int width, int height);

    void draw(GraphicsContext& graphicsContext, Participant* participant);
    void show(void);
    void hide(void);
    void toggle(void);

    void addOnEquipCallback(std::function<void(Item* item, Equippable<Stats::GearStats>::Slot slot)>&& callback);
    void addOnEquipWeaponCallback(std::function<void(Item* item)>&& callback);
    void addOnExamineCallback(std::function<void(Item* item)>&& callback);
};