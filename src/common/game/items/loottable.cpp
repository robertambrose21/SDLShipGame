#include "loottable.h"

LootTable::LootTable()
{ }

LootTable::LootTable(std::vector<LootTableItem> lootTable) :
    lootTable(lootTable)
{ }

const std::vector<LootTableItem> LootTable::getLootTable(void) const {
    return lootTable;
}

std::vector<uint32_t> LootTable::generateItems(void) {
    game_assert(!lootTable.empty());

    std::vector<uint32_t> items;

    for(auto lootTableItem : lootTable) {
        if(randomRange(0, 100) <= lootTableItem.percentChance) {
            auto itemId = generateItem(lootTableItem.itemIds);

            if(itemId != 0) {
                items.push_back(itemId);
            }
        }
    }

    return items;
}

uint32_t LootTable::generateItem(std::vector<uint32_t> itemIds) {
    if(itemIds.empty()) {
        game_assert(false); // We don't want this to happen!
        return 0;
    }

    if(itemIds.size() == 1) {
        return itemIds[0];
    }

    return itemIds[randomRange(0, itemIds.size() - 1)];
}