#pragma once

#include "core/util/timing.h"
#include "applicationcontext.h"
#include "game/weapons/projectilepool.h"
#include "game/weapons/areaofeffectpool.h"
#include "turncontroller.h"
#include "game/entities/behaviour/chaseandattackstrategy.h"
#include "game/weapons/projectileweapon.h"
#include "game/weapons/meleeweapon.h"
#include "game/weapons/weaponcontroller.h"
#include "game/items/itemcontroller.h"

class WeaponController;

class Application {
private:
    std::unique_ptr<Grid> grid;
    std::unique_ptr<EntityPool> entityPool;
    std::unique_ptr<WeaponController> weaponController;
    std::unique_ptr<ProjectilePool> projectilePool;
    std::unique_ptr<AreaOfEffectPool> areaOfEffectPool;
    std::unique_ptr<TurnController> turnController;
    std::unique_ptr<ItemController> itemController;

    std::unique_ptr<ApplicationContext> context;

    std::vector<std::function<void(ApplicationContext&, int64_t, bool&)>> logicWorkers;

public:
    Application(
        std::unique_ptr<Grid> grid,
        std::unique_ptr<EntityPool> entityPool,
        std::unique_ptr<WeaponController> weaponController,
        std::unique_ptr<ProjectilePool> projectilePool,
        std::unique_ptr<AreaOfEffectPool> areaOfEffectPool,
        std::unique_ptr<TurnController> turnController,
        std::unique_ptr<ItemController> itemController
    );

    ApplicationContext& getContext(void);

    void addLogicWorker(std::function<void(ApplicationContext&, int64_t, bool&)> func);
    
    void run(void);
    void loop(void);
};