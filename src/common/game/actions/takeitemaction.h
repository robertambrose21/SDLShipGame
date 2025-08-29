#pragma once

#include "action.h"
#include "game/items/itemcontroller.h"

class TakeItemAction : public Action {
public:
    TakeItemAction(Participant* participant, entt::entity entity, const std::vector<Item*>& items);
    TakeItemAction(Participant* participant, entt::entity entity, int turnNumber, const std::vector<Item*>& items);

    ActionVariant getPublishData(void) override;

    bool passesPrecondition(void);
    Type getType(void);

    const std::vector<Item*>& getItems(void) const;

    private:
    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(ApplicationContext* context);

    bool containsAny(const std::vector<Item*>& itemsA, const std::vector<Item*>& itemsB);
    std::string getItemsAsStringList(void);

    std::vector<Item*> items;
};