#pragma once

#include "action.h"
#include "game/items/itemcontroller.h"

class TakeItemAction : public Action {
private:
    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(void);

    bool containsAny(const std::vector<Item*>& itemsA, const std::vector<Item*>& itemsB);
    std::string getItemsAsStringList(void);

    std::vector<Item*> items;

public:
    TakeItemAction(Participant* participant, Actor* actor, const std::vector<Item*>& items);
    TakeItemAction(Participant* participant, Actor* actor, int turnNumber, const std::vector<Item*>& items);

    ActionVariant getPublishData(void) override;

    bool passesPrecondition(void);
    Type getType(void);

    const std::vector<Item*>& getItems(void) const;
};