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

    std::map<StatsKey::Type, std::vector<StatsPair>> stats;

    std::map<StatsKey::Type, std::vector<StatsPair>> buildStats(void);
    std::string getStatsTypeString(StatsKey::Type type);

    void drawStatsTable(StatsKey::Type type);

public:
    ExamineItemPanel(Item* item);

    void draw(GraphicsContext& graphicsContext);
    bool getIsOpen(void) const;
};