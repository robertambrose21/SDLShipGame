#include "itemcontroller.h"

ItemController::ItemController() {
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

        auto const& textureData = data["texture"].get<json>();
        definition.textureId = textureData["id"].get<uint32_t>();
        auto const& colourData = textureData["colour"].get<json>();
        definition.r = colourData["r"].get<uint8_t>();
        definition.g = colourData["g"].get<uint8_t>();
        definition.b = colourData["b"].get<uint8_t>();
        definition.a = colourData["a"].get<uint8_t>();

        std::cout << "Loaded item definition \"" << definition.name << "\"" << std::endl;

        itemDefinitions[definition.name] = definition;
    }
}

Item* ItemController::addItem(
    const std::string& name, 
    const glm::ivec2& position, 
    Entity* owner,
    bool canPublishEvent
) {
    return addItem(name, position, getNewId(), owner, canPublishEvent);
}

Item* ItemController::addItem(
    const std::string& name, 
    const glm::ivec2& position, 
    uint32_t id,
    Entity* owner,
    bool canPublishEvent
) {
    game_assert(initialised);
    game_assert(itemDefinitions.contains(name));
    game_assert(!items.contains(id));
    
    auto definition = itemDefinitions[name];
    auto item = std::make_unique<Item>(
        definition.name,
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
            owner,
            { items[id].get() }, 
            ItemEventData::SPAWN
        });
    }

    return items[id].get();
}

std::vector<Item*> ItemController::addItems(
    const std::vector<std::string>& itemNames, 
    const glm::ivec2& position,
    Entity* owner
) {
    game_assert(initialised);

    std::vector<Item*> addedItems;

    for(const auto& itemName : itemNames) {
        addedItems.push_back(addItem(itemName, position, owner, false));
    }

    publish<ItemEventData>({ 
        owner,
        addedItems,
        ItemEventData::SPAWN
    });

    return addedItems;
}

std::vector<Item*> ItemController::addItems(
    const std::vector<std::pair<uint32_t, std::string>>& items, 
    const glm::ivec2& position,
    Entity* owner
) {
    game_assert(initialised);

    std::vector<Item*> addedItems;

    for(const auto& [id, name] : items) {
        addedItems.push_back(addItem(name, position, id, owner, false));
    }

    publish<ItemEventData>({
        owner,
        addedItems,
        ItemEventData::SPAWN
    });

    return addedItems;
}

void ItemController::removeItem(uint32_t id) {
    publish<ItemEventData>({
        nullptr, 
        { items[id].get() }, 
        ItemEventData::REMOVED
    });
    
    items.erase(id);
}

void ItemController::removeItems(const std::vector<uint32_t>& ids) {
    std::vector<Item*> itemsToRemove;

    for(auto id : ids) {
        itemsToRemove.push_back(items[id].get());
    }

    publish<ItemEventData>({ nullptr, itemsToRemove, ItemEventData::REMOVED });

    for(auto id : ids) {
        items.erase(id);
    }
}

std::vector<Item*> ItemController::getItemsAt(const glm::ivec2& position) {
    std::vector<Item*> foundItems;

    for(auto& [_, item] : items) {
        if(item->getPosition() == position) {
            foundItems.push_back(item.get());
        }
    }

    return foundItems;
}

Item* ItemController::getItem(uint32_t id) {
    return items[id].get();
}

std::map<uint32_t, std::unique_ptr<Item>> const& ItemController::getItems(void) const {
    return items;
}

void ItemController::onPublish(const Event<EntityEventData>& event) {
    auto entity = event.data.entity;
    auto itemsDropped = context->getEntityPool()->getLootTable(entity->getName()).generateItems();

    if(itemsDropped.empty()) {
        return;
    }    

    addItems(itemsDropped, entity->getPosition(), entity);
}