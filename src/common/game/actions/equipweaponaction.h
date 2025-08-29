#pragma once

#include "action.h"
#include "game/weapons/weapon.h"
#include "game/application/gamecontroller.h"

class EquipWeaponAction : public Action{
public:
    EquipWeaponAction(
        Participant* participant, 
        entt::entity entity, 
        Item* item, 
        const UUID& weaponId,
        bool isUnequip = false
    );
    EquipWeaponAction(
        Participant* participant, 
        entt::entity entity, 
        int turnNumber, 
        Item* item, 
        const UUID& weaponId,
        bool isUnequip = false
    );

    ActionVariant getPublishData(void) override;

    bool passesPrecondition(void);
    Type getType(void);

    Item* getItem(void);
    UUID getWeaponId(void) const;

private:
    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(ApplicationContext* context);

    bool validateEquip(ApplicationContext* context);
    bool validateUnequip(ApplicationContext* context);

    Item* item;
    UUID weaponId;
    bool isUnequip;
};