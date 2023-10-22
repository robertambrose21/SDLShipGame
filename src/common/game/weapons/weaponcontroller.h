#pragma once

#include "weapon.h"
#include "game/entities/entity.h"
#include "core/json.hpp"
#include "projectilepool.h"
#include "core/event/eventpublisher.h"
#include "game/application/applicationcontext.h"

using json = nlohmann::json;

class WeaponController : public EventPublisher<WeaponEventData> {
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

    ApplicationContext* context;
    bool initialised;

public:
    WeaponController();

    void initialise(ApplicationContext& context);

    std::unique_ptr<Weapon> createWeapon(const std::string& name, Entity* owner);
    std::unique_ptr<Weapon> createWeapon(uint32_t id, const std::string& name, Entity* owner);
};