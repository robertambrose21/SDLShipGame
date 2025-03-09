#pragma once

#include "action.h"
#include "game/weapons/weapon.h"

class AttackAction : public Action {
private:
    Weapon* weapon;
    glm::ivec2 target;
    bool isAnimationOnly;

    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(void);

    int numAttacksInChain(void);

public:
    AttackAction(
        Participant* participant,
        Entity* entity,
        Weapon* weapon,
        const glm::ivec2& target,
        bool isAnimationOnly = false
    );
    AttackAction(
        Participant* participant,
        Entity* entity,
        int turnNumber,
        Weapon* weapon,
        const glm::ivec2& target,
        bool isAnimationOnly = false
    );

    bool passesPrecondition(void);
    Weapon* getWeapon(void);
    glm::ivec2 getTarget(void) const;
    Type getType(void);
    bool getIsAnimationOnly(void) const;
};