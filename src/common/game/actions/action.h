#pragma once

#include <optional>

#include "spdlog/spdlog.h"
#include "game/application/applicationcontext.h"
#include "game/participant/participant.h"

class Participant;

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

    Action(Participant* participant, Entity* entity);
    Action(Participant* participant, Entity* entity, int turnNumber);
    virtual ~Action() = default;

    bool validate(ApplicationContext* context);
    bool isFinished(void);
    void execute(ApplicationContext* context);

    virtual bool passesPrecondition(void) = 0;
    virtual Type getType(void) = 0;

    Participant* getParticipant(void);
    Entity* getEntity(void);
    bool isExecuted(void) const;
    std::string typeToString(void);

    std::optional<int> getTurnNumber(void) const;

protected:
    Participant* participant;
    Entity* entity;
    bool _isExecuted;
    std::optional<int> turnNumber;

    virtual bool onValidate(ApplicationContext* context) = 0;
    virtual void onExecute(ApplicationContext* context) = 0;
    virtual bool hasFinished(void) = 0;
};