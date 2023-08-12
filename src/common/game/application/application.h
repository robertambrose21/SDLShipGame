#pragma once

// #include "graphics/window.h"
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
    std::unique_ptr<Window> window;
    std::unique_ptr<Grid> grid;
    std::unique_ptr<EntityPool> entityPool;
    std::unique_ptr<WeaponController> weaponController;
    std::unique_ptr<ProjectilePool> projectilePool;
    std::unique_ptr<AreaOfEffectPool> areaOfEffectPool;
    std::unique_ptr<TurnController> turnController;

    std::unique_ptr<ApplicationContext> context;
    
    Application();
    ~Application();

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
    
    void initialise(const Window::Headless& headless);
    void run(void);
};