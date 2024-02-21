#pragma once

#include "imgui.h"
#include "game/items/item.h"
#include "graphics/graphicscontext.h"
#include "itemraritycolours.h"

class ExamineItemPanel {
private:
    Item* item;
    bool isOpen;

public:
    ExamineItemPanel(Item* item);

    void draw(GraphicsContext& graphicsContext);
    bool getIsOpen(void) const;
};