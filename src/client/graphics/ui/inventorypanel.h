#pragma once

#include "imgui.h"

#include "game/application/turncontroller.h"

class InventoryPanel {
private:
    int width;
    int height;
    bool isShown;

public:
    InventoryPanel(int width, int height);

    void draw(TurnController::Participant* participant);
    void show(void);
    void hide(void);
    void toggle(void);
};