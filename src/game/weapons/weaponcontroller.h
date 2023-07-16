#pragma once

#include "weapon.h"
#include "game/entities/entity.h"
#include "core/json.hpp"
#include "projectilepool.h"

using json = nlohmann::json;

class WeaponController {
private:
    typedef struct _weaponDefinition {
        std::string filename;
        std::string name;
        std::string weaponClass;
        std::string projectile;
        int damage;
        int range;
        int uses;
    } WeaponDefinition;

    void loadWeaponDefinitions(void);

    std::map<std::string, WeaponDefinition> weaponDefinitions;

    Grid& grid;
    ProjectilePool& projectilePool;

public:
    WeaponController(Grid& grid, ProjectilePool& projectilePool);

    std::unique_ptr<Weapon> createWeapon(const std::string& name, Entity* owner);
    std::unique_ptr<Weapon> createWeapon(uint32_t id, const std::string& name, Entity* owner);
};