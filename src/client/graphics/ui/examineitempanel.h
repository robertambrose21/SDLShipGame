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

    std::map<Stats::StatCategory, std::vector<Stats::StatsPair>> stats;

    void drawStatsTable(void);

public:
    ExamineItemPanel(Item* item);

    void draw(GraphicsContext& graphicsContext);
    bool getIsOpen(void) const;
};