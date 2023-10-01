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

class WeaponController;

template<class T>
class Tester {

};

class Application {
private:
    std::unique_ptr<Grid> grid;
    std::unique_ptr<EntityPool> entityPool;
    std::unique_ptr<WeaponController> weaponController;
    std::unique_ptr<ProjectilePool> projectilePool;
    std::unique_ptr<AreaOfEffectPool> areaOfEffectPool;
    std::unique_ptr<TurnController> turnController;

    std::unique_ptr<ApplicationContext> context;

    std::vector<std::function<void(int64_t, bool&)>> logicWorkers;
    
    Application();
    ~Application();

    // int64_t getCurrentTimeInMilliseconds(void);

public:
    static Application& instance() {
        static Application instance;
        return instance;
    }

    Application(Application const&) = delete;
    void operator=(Application const&) = delete;

    static ApplicationContext& getContext(void) {
        return *instance().context;
    }

    static void addLogicWorker(std::function<void(uint32_t, bool&)> func) {
        instance().logicWorkers.push_back(func);
    }
    
    void initialise(void);
    void run(void);
    void loop(void);
};