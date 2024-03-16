#pragma once

#include <vector>
#include <map>

#include "item.h"
#include "core/util/randomutils.h"
#include "core/util/gameassert.h"

typedef struct _lootTableWeightedItem {
    std::string name;
    double weight;
} LootTableWeightedItem;

typedef struct _lootTableItem {
    std::vector<LootTableWeightedItem> items;
    uint8_t percentChance;
    double totalWeights;
} LootTableItem;

class LootTable {
private:
    std::vector<LootTableItem> lootTable;

    std::string generateItem(const std::vector<LootTableWeightedItem>& items, double totalWeights);

public:
    LootTable();
    LootTable(const std::vector<LootTableItem>& lootTable);

    const std::vector<LootTableItem> getLootTable(void) const;

    std::vector<std::string> generateItems(void);
};