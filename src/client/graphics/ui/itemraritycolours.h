#pragma once

#include <map>

#include "imgui.h"

#include "game/items/item.h"

const std::map<Item::Rarity, ImVec4> ItemRarityColours = {
    { Item::Rarity::Unknown, ImVec4(.2f, .2f, .2f, 1) },
    { Item::Rarity::Junk, ImVec4(.5f, .5f, .5f, 1) },
    { Item::Rarity::Common, ImVec4(1, 1, 1, 1) },
    { Item::Rarity::Uncommon, ImVec4(0, 1, 0, 1) },
    { Item::Rarity::Rare, ImVec4(0, 0, 1, 1) },
    { Item::Rarity::Epic, ImVec4(1, 0, 0, 1) }
};