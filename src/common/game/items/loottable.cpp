#include "loottable.h"

LootTable::LootTable()
{ }

LootTable::LootTable(const std::vector<LootTableItem>& lootTable) :
    lootTable(lootTable)
{ }

const std::vector<LootTableItem> LootTable::getLootTable(void) const {
    return lootTable;
}

std::vector<std::string> LootTable::generateItems(void) {
    std::vector<std::string> items;

    for(auto lootTableItem : lootTable) {
        if(randomRange(0, 100) <= lootTableItem.percentChance) {
            items.push_back(generateItem(lootTableItem.items));            
        }
    }

    return items;
}

std::string LootTable::generateItem(const std::vector<std::string>& items) {
    if(items.empty()) {
        game_assert(false); // We don't want this to happen!
        return 0;
    }

    if(items.size() == 1) {
        return items[0];
    }

    return items[randomRange(0, items.size() - 1)];
}