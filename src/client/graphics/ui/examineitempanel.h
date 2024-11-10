#pragma once

#include <map>
#include <vector>

#include "imgui.h"
#include "game/items/item.h"
#include "graphics/graphicscontext.h"
#include "itemraritycolours.h"

class ExamineItemPanel {
private:
    Item* item;
    bool isOpen;

    std::map<StatCategory, std::vector<StatsPair>> stats;

    void drawStatsTable(void);

public:
    ExamineItemPanel(Item* item);

    void draw(GraphicsContext& graphicsContext);
    bool getIsOpen(void) const;
};