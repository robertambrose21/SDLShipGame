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
        std::string item;
        std::string damageSource;
        int power;
        int range;
        int uses;
    } WeaponDefinition;

    std::map<std::string, WeaponDefinition> weaponDefinitions;

    ApplicationContext* context;
    bool initialised;

    void loadWeaponDefinitions(void);
    Item* getItem(const std::string& itemName, Entity* owner);

public:
    WeaponController();

    void initialise(ApplicationContext& context);

    // TODO: Rename to add weapon
    std::unique_ptr<Weapon> createWeapon(const std::string& name, Entity* owner);
    std::unique_ptr<Weapon> createWeapon(const UUID& id, const std::string& name, Entity* owner);
};
