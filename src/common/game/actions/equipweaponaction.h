#pragma once

#include "action.h"
#include "game/weapons/weapon.h"
#include "game/application/turncontroller.h"

class EquipWeaponAction : public Action{
private:
    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(void);

    Item* item;
    UUID weaponId;

public:
    EquipWeaponAction(int turnNumber, Entity* entity, Item* item, const UUID& weaponId);

    bool passesPrecondition(void);
    Type getType(void);

    Item* getItem(void);
    UUID getWeaponId(void) const;
};