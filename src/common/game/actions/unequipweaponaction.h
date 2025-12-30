#pragma once

#include "action.h"
#include "game/weapons/weapon.h"
#include "game/application/gamecontroller.h"

class UnequipWeaponAction : public Action{
public:
    UnequipWeaponAction(
        Participant* participant, 
        entt::entity entity, 
        Item* item, 
        entt::entity weaponId
    );
    UnequipWeaponAction(
        Participant* participant, 
        entt::entity entity, 
        int turnNumber, 
        Item* item, 
        entt::entity weaponId
    );

    ActionVariant getPublishData(void) override;

    bool passesPrecondition(ApplicationContext* context);
    Type getType(void);

    Item* getItem(void);
    entt::entity getWeaponId(void) const;

private:
    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(ApplicationContext* context);

    Item* item;
    entt::entity weaponId;
};