#pragma once

#include "action.h"
#include "game/weapons/weapon.h"

class AttackAction : public Action {
public:
    AttackAction(
        Participant* participant,
        entt::entity entity,
        entt::entity weaponId,
        const glm::ivec2& target,
        bool isAnimationOnly = false
    );
    AttackAction(
        Participant* participant,
        entt::entity entity,
        int turnNumber,
        entt::entity weaponId,
        const glm::ivec2& target,
        bool isAnimationOnly = false
    );

    ActionVariant getPublishData(void) override;

    bool passesPrecondition(ApplicationContext* context);
    entt::entity getWeapon(void);
    glm::ivec2 getTarget(void) const;
    Type getType(void);
    bool getIsAnimationOnly(void) const;

private:
    entt::entity weaponId;
    glm::ivec2 target;
    bool isAnimationOnly;

    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(ApplicationContext* context);

    int numAttacksInChain(ApplicationContext* context);
};