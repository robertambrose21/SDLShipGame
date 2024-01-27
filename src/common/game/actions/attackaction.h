#pragma once

#include "action.h"

class AttackAction : public Action {
private:
    Weapon* weapon;
    glm::ivec2 target;

    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(void);

    int numAttacksInChain(void);

public:
    AttackAction(
        int turnNumber,
        Entity* entity,
        Weapon* weapon,
        const glm::ivec2& target
    );

    bool passesPrecondition(void);
    Weapon* getWeapon(void);
    glm::ivec2 getTarget(void) const;
    Type getType(void);
};