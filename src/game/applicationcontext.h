#pragma once

#include "../graphics/window.h"
#include "entitypool.h"
#include "projectilepool.h"
#include "areaofeffectpool.h"
#include "turncontroller.h"

class ApplicationContext {
private:
    std::shared_ptr<Window> window;
    std::shared_ptr<EntityPool> entityPool;
    std::shared_ptr<ProjectilePool> projectilePool;
    std::shared_ptr<AreaOfEffectPool> areaOfEffectPool;
    std::shared_ptr<TurnController> turnController;

public:
    ApplicationContext(
        std::shared_ptr<Window> window,
        std::shared_ptr<EntityPool> entityPool,
        std::shared_ptr<ProjectilePool> projectilePool,
        std::shared_ptr<AreaOfEffectPool> areaOfEffectPool,
        std::shared_ptr<TurnController> turnController
    );

    std::shared_ptr<Window> getWindow(void);
    std::shared_ptr<EntityPool> getEntityPool(void);
    std::shared_ptr<ProjectilePool> getProjectilePool(void);
    std::shared_ptr<AreaOfEffectPool> getAreaOfEffectPool(void);
    std::shared_ptr<TurnController> getTurnController(void);
};