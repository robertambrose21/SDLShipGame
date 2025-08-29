#pragma once

#include <optional>
#include <any>
#include <variant>
#include <entt/entt.hpp>

#include "spdlog/spdlog.h"

#include "game/application/applicationcontext.h"
#include "game/participant/participant.h"
#include "game/engagements/engagement.h"
#include "game/actors/actor.h"


using ActionVariant = std::variant<
    MoveActionEventData,
    AttackActionEventData,
    TakeItemActionEventData,
    EquipItemActionEventData,
    EquipWeaponActionEventData
>; 

class Action {
public:
    enum Type {
        Move = 0,
        Attack,
        TakeItem,
        EquipItem,
        EquipWeaponItem,
        // Freeze,
        Count
    };

    Action(Participant* participant, entt::entity entity);
    Action(Participant* participant, entt::entity entity, int turnNumber);
    virtual ~Action() = default;

    virtual ActionVariant getPublishData(void) = 0;

    bool validate(ApplicationContext* context);
    bool isFinished(ApplicationContext* context);
    void execute(ApplicationContext* context);

    virtual bool passesPrecondition(void) = 0;
    virtual Type getType(void) = 0;

    Participant* getParticipant(void);
    entt::entity getEntity(void) const;
    bool isExecuted(void) const;
    std::string typeToString(void);

    std::optional<int> getTurnNumber(void) const;

protected:
    Participant* participant;
    entt::entity entity;
    bool _isExecuted;
    std::optional<int> turnNumber;

    virtual bool onValidate(ApplicationContext* context) = 0;
    virtual void onExecute(ApplicationContext* context) = 0;
    virtual bool hasFinished(ApplicationContext* context) = 0;
};