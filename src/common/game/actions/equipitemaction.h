#pragma once

#include "action.h"
#include "game/items/equipment.h"
#include "game/application/turncontroller.h"

class EquipItemAction : public Action {
private:
    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(void);

    Item* item;
    Equipment::Slot slot;
    bool isUnequip;

public:
    EquipItemAction(int turnNumber, Entity* entity, Item* item, Equipment::Slot slot, bool isUnequip = false);

    bool passesPrecondition(void);
    Type getType(void);

    Item* getItem(void);
    Equipment::Slot getSlot(void) const;
    bool getIsUnequip(void) const;
};
