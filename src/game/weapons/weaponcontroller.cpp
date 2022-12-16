#include "weaponcontroller.h"
#include "projectileweapon.h"

WeaponController::WeaponController(const std::shared_ptr<Grid>& grid, const std::shared_ptr<ProjectilePool>& projectilePool) :
    grid(grid),
    projectilePool(projectilePool)
{
   loadWeaponDefinitions();
}

void WeaponController::loadWeaponDefinitions(void) {
    std::string directory = "../assets/data/weapons";

    for(const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::ifstream f(entry.path());
        json data = json::parse(f);

        WeaponDefinition definition;
        definition.filename = entry.path();
        definition.name = data["name"].get<std::string>();
        definition.weaponClass = data["class"].get<std::string>();
        if(data.contains("projectile")) {
            definition.projectile = data["projectile"].get<std::string>();
        }
        else {
            definition.projectile = "";
        }
        definition.damage = data["damage"].get<int>();
        definition.range = data["range"].get<int>();
        definition.uses = data["uses"].get<int>();

        std::cout << "Loaded weapon definition \"" << definition.name << "\"" << std::endl;

        weaponDefinitions[definition.name] = definition;
    }
}

std::shared_ptr<Weapon> WeaponController::createWeapon(
    const uint32_t& id, 
    const std::string& name, 
    const std::shared_ptr<Entity>& owner
) {
    game_assert(weaponDefinitions.contains(name));
    auto definition = weaponDefinitions[name];
    
    if(definition.weaponClass == "Projectile") {
        return std::make_shared<ProjectileWeapon>(
            owner,
            grid,
            id,
            definition.name,
            Weapon::Stats { definition.damage, definition.range, definition.uses },
            projectilePool->create(definition.projectile)
        );
    }
    else if(definition.weaponClass == "Melee") {
        return std::make_shared<MeleeWeapon>(
            owner,
            grid,
            id,
            definition.name,
            Weapon::Stats { definition.damage, definition.range, definition.uses }
        );
    }

    throw std::runtime_error("Could not create weapon of class \"" + definition.weaponClass + "\"");
}

std::shared_ptr<Weapon> WeaponController::createWeapon(const std::string& name, const std::shared_ptr<Entity>& owner) {
    return createWeapon(getNewId(), name, owner);
}