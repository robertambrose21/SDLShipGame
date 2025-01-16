#include "itemcontroller.h"

const std::map<std::string, Item::Rarity> ItemController::StringToRarity = {
    { "Junk", Item::Rarity::Junk },
    { "Common", Item::Rarity::Common },
    { "Uncommon", Item::Rarity::Uncommon },
    { "Rare", Item::Rarity::Rare },
    { "Epic", Item::Rarity::Epic }
};

ItemController::ItemController() :
    areWorldItemsDirty(true),
    initialised(false)
{
    loadItemDefinitions();
}

void ItemController::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

void ItemController::loadItemDefinitions(void) {
    std::string directory = "../assets/data/items";

    for(const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::ifstream f(entry.path());
        json data = json::parse(f);

        ItemDefinition definition;
        definition.filename = entry.path();
        definition.name = data["name"].get<std::string>();
        definition.type = data["type"].get<std::string>();
        definition.rarity = data["rarity"].get<std::string>();

        auto const& textureData = data["texture"].get<json>();
        definition.textureId = textureData["id"].get<uint32_t>();
        auto const& colourData = textureData["colour"].get<json>();
        definition.r = colourData["r"].get<uint8_t>();
        definition.g = colourData["g"].get<uint8_t>();
        definition.b = colourData["b"].get<uint8_t>();
        definition.a = colourData["a"].get<uint8_t>();

        if(data.contains("stats")) {
            auto const& stats = data["stats"].get<json>();

            if(isGear(definition.type)) {
                if(stats.contains("speed")) definition.stats.gear.speed = stats["speed"].get<int>();
                if(stats.contains("hp")) definition.stats.gear.hp = stats["hp"].get<int>();
                if(stats.contains("armour")) definition.stats.gear.armour = stats["armour"].get<int>();
            }
            else if(isWeapon(definition.type)) {
                if(stats.contains("speed")) definition.stats.weapon.speed = stats["speed"].get<int>();
                if(stats.contains("hp")) definition.stats.weapon.hp = stats["hp"].get<int>();
                if(stats.contains("armour")) definition.stats.weapon.armour = stats["armour"].get<int>();
            }
        }

        std::cout << "Loaded item definition \"" << definition.name << "\"" << std::endl;

        itemDefinitions[definition.name] = definition;
    }
}

Item::Rarity ItemController::mapToRarity(const std::string& rarityString) {
    if(!StringToRarity.contains(rarityString)) {
        return Item::Rarity::Unknown;
    }

    return StringToRarity.at(rarityString);
}

bool ItemController::isGear(const std::string& type) {
    auto slot = Equippable<Stats::GearStats>::getSlotFromName(type);
    return contains(Gear::VALID_SLOTS, slot);
}

bool ItemController::isWeapon(const std::string& type) {
    return type == "Weapon";
}

Item* ItemController::addItem(
        const std::string& name, 
        const glm::ivec2& position, 
        const std::string& droppedBy,
        bool canPublishEvent
) {
    return addItem(name, position, getNewId(), droppedBy, canPublishEvent);
}

Item* ItemController::addItem(
    const std::string& name, 
    const glm::ivec2& position, 
    uint32_t id,
    const std::string& droppedBy,
    bool canPublishEvent
) {
    game_assert(initialised);
    game_assert(itemDefinitions.contains(name));

    if(items.contains(id)) {
        spdlog::warn("Item {}#{} already exists, duplicate id {}. Retrieving existing item", name, id, id);
        return items[id].get();
    }
    
    auto definition = itemDefinitions[name];
    auto item = std::make_unique<Item>(
        definition.name,
        droppedBy,
        mapToRarity(definition.rarity),
        definition.stats,
        definition.type,
        position,
        id
    );
    item->setTextureId(definition.textureId);
    item->setColour({
        definition.r,
        definition.g,
        definition.b,
        definition.a
    });

    items[id] = std::move(item);

    if(canPublishEvent) {
        publish<ItemEventData>({
            droppedBy,
            { items[id].get() }, 
            ItemEventData::SPAWN
        });
    }

    areWorldItemsDirty = true;

    spdlog::trace(
        "Added item {}#{} at position ({}, {}), dropped by: {}",
        name,
        id,
        position.x,
        position.y,
        droppedBy
    );

    return items[id].get();
}

std::vector<Item*> ItemController::addItems(
    const std::vector<std::string>& itemNames, 
    const glm::ivec2& position,
    const std::string& droppedBy
) {
    game_assert(initialised);

    std::vector<Item*> addedItems;

    for(const auto& itemName : itemNames) {
        addedItems.push_back(addItem(itemName, position, droppedBy, false));
    }

    publish<ItemEventData>({ 
        droppedBy,
        addedItems,
        ItemEventData::SPAWN
    });

    return addedItems;
}

std::vector<Item*> ItemController::addItems(
    const std::vector<std::pair<uint32_t, std::string>>& items, 
    const glm::ivec2& position,
    const std::string& droppedBy
) {
    game_assert(initialised);

    std::vector<Item*> addedItems;

    for(const auto& [id, name] : items) {
        addedItems.push_back(addItem(name, position, id, droppedBy, false));
    }

    publish<ItemEventData>({
        droppedBy,
        addedItems,
        ItemEventData::SPAWN
    });

    return addedItems;
}

void ItemController::removeItem(uint32_t id) {
    publish<ItemEventData>({
        Item::UnknownOwner, 
        { items[id].get() }, 
        ItemEventData::REMOVED
    });
    
    items.erase(id);

    areWorldItemsDirty = true;
}

void ItemController::removeItems(const std::vector<uint32_t>& ids) {
    std::vector<Item*> itemsToRemove;

    for(auto id : ids) {
        itemsToRemove.push_back(items[id].get());
    }

    publish<ItemEventData>({ Item::UnknownOwner, itemsToRemove, ItemEventData::REMOVED });

    for(auto id : ids) {
        items.erase(id);
    }

    areWorldItemsDirty = true;
}

void ItemController::flagWorldItemsDirty(void) {
    areWorldItemsDirty = true;
}

std::vector<Item*> ItemController::getItemsAt(const glm::ivec2& position) {
    std::vector<Item*> foundItems;

    for(auto item : getWorldItems()) {
        if(item->getPosition() == position) {
            foundItems.push_back(item);
        }
    }

    return foundItems;
}

Item* ItemController::getItem(uint32_t id) {
    return items[id].get();
}

bool ItemController::hasItem(uint32_t id) {
    return items.contains(id);
}

std::map<uint32_t, std::unique_ptr<Item>> const& ItemController::getItems(void) const {
    return items;
}

std::vector<Item*> ItemController::getWorldItems(void) {
    if(areWorldItemsDirty) {
        worldItems.clear();

        for(auto& [_, item] : items) { 
            if(item->getParticipantId() == -1) {
                worldItems.push_back(item.get());
            }
        }

        areWorldItemsDirty = false;
    }

    return worldItems;
}

void ItemController::onPublish(const Event<EntityEventData>& event) {
    auto entity = event.data.entity;
    auto itemsDropped = context->getEntityPool()->getLootTable(entity->getName()).generateItems();

    if(itemsDropped.empty()) {
        return;
    }    

    addItems(itemsDropped, entity->getPosition(), entity == nullptr ? Item::UnknownOwner : entity->toString());
}