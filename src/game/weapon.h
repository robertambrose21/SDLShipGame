#pragma once

#include <string>
#include <functional>
#include <memory>
#include "entity.h"

class Entity;

class Weapon {
public:
    typedef struct _stats {
        int damage;
        int range;
        int rounds;
    } Stats;

private:
    std::string name;
    Stats stats;

    std::function<void(void)> onHitEvent;

public:
    Weapon(const std::string& name, Stats stats);

    void use(std::shared_ptr<Entity> target);

    Stats getStats(void) const;
    std::string getName(void) const;
};