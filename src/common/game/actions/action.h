#pragma once

#include "spdlog/spdlog.h"
#include "game/application/applicationcontext.h"

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

protected:
    Entity* entity;
    bool _isExecuted;
    int turnNumber;

    virtual bool onValidate(ApplicationContext* context) = 0;
    virtual void onExecute(ApplicationContext* context) = 0;
    virtual bool hasFinished(void) = 0;

public:
    Action(int turnNumber, Entity* entity);
    virtual ~Action() = default;

    bool validate(ApplicationContext* context);
    bool isFinished(void);
    void execute(ApplicationContext* context);

    virtual bool passesPrecondition(void) = 0;
    virtual Type getType(void) = 0;

    Entity* getEntity(void);
    bool isExecuted(void) const;
    int getTurnNumber(void) const;
    std::string typeToString(void);
};