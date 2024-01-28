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
            items.push_back(generateItem(lootTableItem.items, lootTableItem.totalWeights));            
        }
    }

    return items;
}

std::string LootTable::generateItem(const std::vector<LootTableWeightedItem>& items, double totalWeights) {
    if(items.empty()) {
        game_assert(false); // We don't want this to happen!
        return 0;
    }

    if(items.size() == 1) {
        return items[0].name;
    }

    std::map<std::string, double> weightMap;
    auto val = randomRangeDouble(0.0, totalWeights);
    double weightsSoFar = 0.0;

    for(auto item : items) {
        weightsSoFar += item.weight;

        if(val <= weightsSoFar) {
            return item.name;
        }
    }

    return items[0].name;
}