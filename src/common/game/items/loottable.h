#pragma once

#include <vector>

#include "item.h"
#include "core/util/randomrolls.h"
#include "core/util/gameassert.h"

typedef struct _lootTableItem {
    std::vector<std::string> items;
    uint8_t percentChance;
} LootTableItem;

class LootTable {
private:
    std::vector<LootTableItem> lootTable;

    std::string generateItem(const std::vector<std::string>& items);

public:
    LootTable();
    LootTable(const std::vector<LootTableItem>& lootTable);

    const std::vector<LootTableItem> getLootTable(void) const;

    std::vector<std::string> generateItems(void);
};