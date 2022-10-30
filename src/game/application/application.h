#pragma once

#include "graphics/window.h"
#include "player/playercontroller.h"
#include "applicationcontext.h"
#include "game/weapons/projectilepool.h"
#include "game/weapons/areaofeffectpool.h"
#include "turncontroller.h"

#include "game/entities/enemy.h"
#include "game/weapons/projectileweapon.h"
#include "game/weapons/meleeweapon.h"

class PlayerController;

class Application {
private:
    std::shared_ptr<Window> window;
    std::shared_ptr<Grid> grid;
    std::shared_ptr<EntityPool> entityPool;
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
    
    void initialise(Window::Headless headless);
    void run(void);

    // Temp
    std::shared_ptr<Entity> addPlayer(glm::ivec2 position);
};