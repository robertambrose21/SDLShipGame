#pragma once

#include <map>
#include <vector>

#include "imgui.h"
#include "game/items/item.h"
#include "graphics/graphicscontext.h"
#include "itemraritycolours.h"

class ExamineItemPanel {
private:
    typedef struct _statsPair {
        std::string name;
        std::string value;
    } StatsPair;

    Item* item;
    bool isOpen;

    std::map<StatsValue::Type, std::vector<StatsPair>> stats;

    std::map<StatsValue::Type, std::vector<StatsPair>> buildStats(void);
    std::string getStatsTypeString(StatsValue::Type type);

    void drawStatsTable(StatsValue::Type type);

public:
    ExamineItemPanel(Item* item);

    void draw(GraphicsContext& graphicsContext);
    bool getIsOpen(void) const;
};