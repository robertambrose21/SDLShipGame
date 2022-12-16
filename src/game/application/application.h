#pragma once

#include "graphics/window.h"
#include "applicationcontext.h"
#include "game/weapons/projectilepool.h"
#include "game/weapons/areaofeffectpool.h"
#include "turncontroller.h"
#include "game/entities/behaviour/chaseandattackstrategy.h"
#include "game/weapons/projectileweapon.h"
#include "game/weapons/meleeweapon.h"
#include "game/weapons/weaponcontroller.h"

class WeaponController;

class Application {
private:
    std::shared_ptr<Window> window;
    std::shared_ptr<Grid> grid;
    std::shared_ptr<EntityPool> entityPool;
    std::shared_ptr<WeaponController> weaponController;
    std::shared_ptr<ProjectilePool> projectilePool;
    std::shared_ptr<AreaOfEffectPool> areaOfEffectPool;
    std::shared_ptr<TurnController> turnController;

    std::shared_ptr<ApplicationContext> context;
    
    Application();
    ~Application();

public:
    static Application& instance() {
        static Application instance;
        return instance;
    }

    Application(Application const&) = delete;
    void operator=(Application const&) = delete;

    static std::shared_ptr<ApplicationContext> getContext(void) {
        return instance().context;
    }
    
    void initialise(const Window::Headless& headless);
    void run(void);
};