#pragma once

#include "action.h"
#include "game/items/itemcontroller.h"

class TakeItemAction : public Action {
private:
    bool onValidate(void);
    void onExecute(ApplicationContext* context);
    bool hasFinished(void);

    bool containsAny(std::vector<Item*> itemsA, std::vector<Item*> itemsB);

    std::vector<Item*> items;

public:
    TakeItemAction(int turnNumber, Entity* entity, const std::vector<Item*>& items);

    bool passesPrecondition(void);
    Type getType(void);

    const std::vector<Item*>& getItems(void) const;
};