#pragma once

#include "action.h"
#include "game/items/equippable.h"
#include "game/application/turncontroller.h"

class EquipItemAction : public Action {
private:
    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(void);

    Item* item;
    Equippable<GearStats>::Slot slot;
    bool isUnequip;

public:
    EquipItemAction(
        int turnNumber, 
        Entity* entity, 
        Item* item, 
        Equippable<GearStats>::Slot slot, 
        bool isUnequip = false
    );

    bool passesPrecondition(void);
    Type getType(void);

    Item* getItem(void);
    Equippable<GearStats>::Slot getSlot(void) const;
    bool getIsUnequip(void) const;
};
