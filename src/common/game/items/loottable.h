#pragma once

#include <vector>

#include "item.h"
#include "core/util/randomrolls.h"
#include "core/util/gameassert.h"

typedef struct _lootTableItem {
    std::vector<uint32_t> itemIds;
    uint8_t percentChance;
} LootTableItem;

class LootTable {
private:
    std::vector<LootTableItem> lootTable;

    uint32_t generateItem(std::vector<uint32_t> itemIds);

public:
    LootTable();
    LootTable(std::vector<LootTableItem> lootTable);

    const std::vector<LootTableItem> getLootTable(void) const;

    std::vector<uint32_t> generateItems(void);
};