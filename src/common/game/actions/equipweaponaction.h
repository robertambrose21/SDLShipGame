#pragma once

#include "action.h"
#include "game/weapons/weapon.h"
#include "game/application/gamecontroller.h"

class EquipWeaponAction : public Action{
private:
    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(void);

    Item* item;
    UUID weaponId;

public:
    EquipWeaponAction(Participant* participant, Entity* entity, Item* item, const UUID& weaponId);
    EquipWeaponAction(Participant* participant, Entity* entity, int turnNumber, Item* item, const UUID& weaponId);

    void publish(ActionPublisher& publisher) override;

    bool passesPrecondition(void);
    Type getType(void);

    Item* getItem(void);
    UUID getWeaponId(void) const;
};