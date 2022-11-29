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

    std::shared_ptr<Grid> grid;
    std::shared_ptr<ProjectilePool> projectilePool;

public:
    WeaponController(std::shared_ptr<Grid> grid, std::shared_ptr<ProjectilePool> projectilePool);

    std::shared_ptr<Weapon> createWeapon(const std::string& name, std::shared_ptr<Entity> owner);
    std::shared_ptr<Weapon> createWeapon(const uint32_t& id, const std::string& name, std::shared_ptr<Entity> owner);
};