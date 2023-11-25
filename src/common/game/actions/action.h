#pragma once

#include "game/application/applicationcontext.h"

class Action {
public:
    enum Type {
        Move = 0,
        Attack,
        // Freeze,
        Count
    };

protected:
    Entity* entity;
    bool _isExecuted;

    virtual bool onValidate(void) = 0;
    virtual void onExecute(ApplicationContext* context) = 0;
    virtual bool hasFinished(void) = 0;

public:
    Action(Entity* entity);

    bool validate(void);
    bool isFinished(void);
    void execute(ApplicationContext* context);

    virtual Type getType(void) = 0;

    Entity* getEntity(void);
    bool isExecuted(void) const;
};