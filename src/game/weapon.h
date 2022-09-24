#pragma once

#include <string>
#include <functional>
#include <memory>

#include "entity.h"
#include "projectile.h"

class Entity;
class Projectile;

class Weapon {
public:
    typedef struct _stats {
        int damage;
        int range;
        int uses;
    } Stats;

protected:
    std::string name;
    Stats stats;

    int usesLeft;

    std::shared_ptr<GridRenderer> gridRenderer;

    virtual void onUse(glm::ivec2 position, std::shared_ptr<Entity> target) = 0;

public:
    Weapon(std::shared_ptr<GridRenderer> gridRenderer, const std::string& name, Stats stats);

    void use(glm::ivec2 position, std::shared_ptr<Entity> target);
    void reset(void);

    virtual void draw(std::shared_ptr<SDL_Renderer> renderer) = 0;
    virtual void update(const Uint32& timeSinceLastFrame) = 0;

    Stats getStats(void) const;
    int getUsesLeft(void) const;

    std::string getName(void) const;
};