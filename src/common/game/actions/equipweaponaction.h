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
        const UUID& weaponUUID,
        bool isUnequip = false
    );
    EquipWeaponAction(
        Participant* participant, 
        entt::entity entity, 
        int turnNumber, 
        Item* item, 
        const UUID& weaponUUID,
        bool isUnequip = false
    );

    ActionVariant getPublishData(void) override;

    bool passesPrecondition(ApplicationContext* context);
    Type getType(void);

    Item* getItem(void);
    UUID getWeaponUUID(void) const;

private:
    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(ApplicationContext* context);

    bool validateEquip(ApplicationContext* context, Actor* actor);
    bool validateUnequip(ApplicationContext* context, Actor* actor);

    Item* item;
    UUID weaponUUID;
    entt::entity weaponId;
    bool isUnequip;
};