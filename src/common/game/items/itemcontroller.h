#pragma once

#include <map>
#include <fstream>
#include <filesystem>

#include "core/json.hpp"
#include "core/event/eventpublisher.h"
#include "core/event/eventsubscriber.h"
#include "game/application/applicationcontext.h"
#include "game/entities/entitypool.h"
#include "item.h"

using json = nlohmann::json;

class ItemController : 
    public EventPublisher<ItemEventData>,
    public EventSubscriber<EntityEventData>
{
public:
    ItemController();

    void initialise(ApplicationContext& context);

    Item* addItem(
        const std::string& name, 
        const glm::ivec2& position, 
        const std::string& droppedBy,
        bool canPublishEvent = true
    );
    Item* addItem(
        const std::string& name, 
        const glm::ivec2& position, 
        uint32_t id, 
        const std::string& droppedBy,
        bool canPublishEvent = true
    );
    std::vector<Item*> addItems(
        const std::vector<std::string>& itemNames, 
        const glm::ivec2& position, 
        const std::string& droppedBy
    );
    std::vector<Item*> addItems(
        const std::vector<std::pair<uint32_t, std::string>>& items, 
        const glm::ivec2& position, 
        const std::string& droppedBy
    );

    void removeItem(uint32_t id);
    void removeItems(const std::vector<uint32_t>& ids);
    std::vector<Item*> getItemsAt(const glm::ivec2& position);
    bool hasItem(uint32_t id);
    Item* getItem(uint32_t id);

    void flagWorldItemsDirty(void);

    std::map<uint32_t, std::unique_ptr<Item>> const& getItems(void) const;
    std::vector<Item*> getWorldItems(void);

    void onPublish(const Event<EntityEventData>& event);

private:
    typedef struct itemDefinition {
        std::string filename;
        std::string name;
        std::string type;
        std::string rarity;
        uint32_t textureId;
        uint8_t r, g, b, a;
        Stats::ItemStats stats;
    } ItemDefinition;

    static const std::map<std::string, Item::Rarity> StringToRarity;

    std::map<std::string, ItemDefinition> itemDefinitions;
    std::map<uint32_t, std::unique_ptr<Item>> items;
    std::vector<Item*> worldItems;
    bool areWorldItemsDirty;

    ApplicationContext* context;
    bool initialised;

    void loadItemDefinitions(void);

    Item::Rarity mapToRarity(const std::string& rarityString);
    bool isGear(const std::string& type);
    bool isWeapon(const std::string& type);
};