#pragma once

#include "action.h"
#include "game/items/equippable.h"
#include "game/application/turncontroller.h"

class EquipGearAction : public Action {
private:
    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(void);

    Item* item;
    Equippable<Stats::GearStats>::Slot slot;
    bool isUnequip;

public:
    EquipGearAction(
        int turnNumber, 
        Entity* entity, 
        Item* item, 
        Equippable<Stats::GearStats>::Slot slot, 
        bool isUnequip = false
    );

    bool passesPrecondition(void);
    Type getType(void);

    Item* getItem(void);
    Equippable<Stats::GearStats>::Slot getSlot(void) const;
    bool getIsUnequip(void) const;
};
