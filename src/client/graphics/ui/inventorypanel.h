#pragma once

#include "imgui.h"
#include "itemraritycolours.h"

#include "graphics/graphicscontext.h"
#include "game/application/turncontroller.h"

class InventoryPanel {
private:
    int width;
    int height;
    bool isShown;

public:
    InventoryPanel(int width, int height);

    void draw(GraphicsContext& graphicsContext, TurnController::Participant* participant);
    void show(void);
    void hide(void);
    void toggle(void);
};