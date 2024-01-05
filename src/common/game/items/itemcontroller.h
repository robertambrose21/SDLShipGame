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
private:
    typedef struct itemDefinition {
        std::string filename;
        std::string name;
        std::string type;
        uint32_t textureId;
        uint8_t r, g, b, a;
    } ItemDefinition;

    std::map<std::string, ItemDefinition> itemDefinitions;
    std::map<uint32_t, std::unique_ptr<Item>> items;

    ApplicationContext* context;
    bool initialised;

    void loadItemDefinitions(void);

public:
    ItemController();

    void initialise(ApplicationContext& context);

    Item* addItem(const std::string& name, const glm::ivec2& position, Entity* owner, bool canPublishEvent = true);
    Item* addItem(const std::string& name, const glm::ivec2& position, uint32_t id, Entity* owner, bool canPublishEvent = true);
    std::vector<Item*> addItems(const std::vector<std::string>& itemNames, const glm::ivec2& position, Entity* owner);
    std::vector<Item*> addItems(const std::vector<std::pair<uint32_t, std::string>>& items, const glm::ivec2& position, Entity* owner);

    void removeItem(uint32_t id);
    void removeItems(const std::vector<uint32_t>& ids);
    std::vector<Item*> getItemsAt(const glm::ivec2& position);
    Item* getItem(uint32_t id);

    std::map<uint32_t, std::unique_ptr<Item>> const& getItems(void) const;

    void onPublish(const Event<EntityEventData>& event);
};