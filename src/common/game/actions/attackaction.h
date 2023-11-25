#pragma once

#include "action.h"

class AttackAction : public Action {
private:
    Weapon* weapon;
    glm::ivec2 target;

    bool onValidate(void);
    void onExecute(ApplicationContext* context);
    bool hasFinished(void);

    int numAttacksInChain(void);

public:
    AttackAction(
        Entity* entity,
        Weapon* weapon,
        const glm::ivec2& target
    );

    bool passesPrecondition(void);
    Weapon* getWeapon(void);
    Type getType(void);
};