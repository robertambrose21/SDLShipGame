#pragma once

#include "action.h"
#include "game/items/equippable.h"
#include "game/application/gamecontroller.h"

class EquipGearAction : public Action {
private:
    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(ApplicationContext* context);

    Item* item;
    Equippable<Stats::GearStats>::Slot slot;
    bool isUnequip;

public:
    EquipGearAction(
        Participant* participant, 
        entt::entity entity, 
        Item* item, 
        Equippable<Stats::GearStats>::Slot slot, 
        bool isUnequip = false
    );
    EquipGearAction(
        Participant* participant, 
        entt::entity entity,
        int turnNumber,
        Item* item, 
        Equippable<Stats::GearStats>::Slot slot, 
        bool isUnequip = false
    );

    ActionVariant getPublishData(void) override;

    bool passesPrecondition(ApplicationContext* context);
    Type getType(void);

    Item* getItem(void);
    Equippable<Stats::GearStats>::Slot getSlot(void) const;
    bool getIsUnequip(void) const;
};
