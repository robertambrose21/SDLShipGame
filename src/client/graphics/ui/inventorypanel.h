#pragma once

#include <functional>

#include "imgui.h"
#include "itemraritycolours.h"

#include "game/items/equipment.h"
#include "graphics/graphicscontext.h"
#include "game/application/turncontroller.h"

class InventoryPanel {
private:
    int width;
    int height;
    bool isShown;

    std::function<void(Item* item, Equipment::Slot slot)> onEquipClicked;
    std::function<void(Item* item, Equipment::Slot slot)> onUnequipClicked;
    std::function<void(Item* item)> onExamineClicked;

    void drawItem(GraphicsContext& graphicsContext, Item* item, bool isEquipped);

public:
    InventoryPanel(int width, int height);

    void draw(GraphicsContext& graphicsContext, TurnController::Participant* participant);
    void show(void);
    void hide(void);
    void toggle(void);

    void addOnEquipCallback(std::function<void(Item* item, Equipment::Slot slot)>&& callback);
    void addOnUnequipCallback(std::function<void(Item* item, Equipment::Slot slot)>&& callback);
    void addOnExamineCallback(std::function<void(Item* item)>&& callback);
};