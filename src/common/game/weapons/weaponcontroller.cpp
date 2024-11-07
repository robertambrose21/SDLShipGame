#include "weaponcontroller.h"
#include "projectileweapon.h"

WeaponController::WeaponController() {
   loadWeaponDefinitions();
}

void WeaponController::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}


void WeaponController::loadWeaponDefinitions(void) {
    std::string directory = "../assets/data/weapons";

    for(const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::ifstream f(entry.path());
        json data = json::parse(f);

        WeaponDefinition definition;
        definition.filename = entry.path();
        definition.name = data["name"].get<std::string>();
        if(data.contains("item")) {
            definition.item = data["item"].get<std::string>();
        }
        else {
            definition.item = "";
        }
        definition.weaponClass = data["class"].get<std::string>();
        if(data.contains("projectile")) {
            definition.projectile = data["projectile"].get<std::string>();
        }
        else {
            definition.projectile = "";
        }
        
        definition.damageSource = data["damage"].get<std::string>();
        definition.range = data["range"].get<int>();
        definition.uses = data["uses"].get<int>();
        definition.power = data["power"].get<int>();

        std::cout << "Loaded weapon definition \"" << definition.name << "\"" << std::endl;

        weaponDefinitions[definition.name] = definition;
    }
}

std::unique_ptr<Weapon> WeaponController::createWeapon(
    const UUID& id,
    const std::string& name, 
    Entity* owner
) {
    game_assert(initialised);
    game_assert(weaponDefinitions.contains(name));

    auto definition = weaponDefinitions[name];
    auto item = getItem(definition.item, owner);
    auto damageSource = DamageSource::parse(definition.damageSource, definition.power);

    if(definition.weaponClass == "Projectile") {
        auto projectileBlueprint = context->getProjectilePool()->create(definition.projectile);

        return std::make_unique<ProjectileWeapon>(
            owner,
            context,
            item,
            *this,
            id,
            definition.name,
            buildProjectileWeaponStats(definition, projectileBlueprint, damageSource, item),
            damageSource,
            projectileBlueprint
        );
    }
    else if(definition.weaponClass == "Melee") {
        return std::make_unique<MeleeWeapon>(
            owner,
            context,
            item,
            *this,
            id,
            definition.name,
            damageSource,
            buildMeleeWeaponStats(definition, damageSource, item)
        );
    }

    throw std::runtime_error("Could not create weapon of class \"" + definition.weaponClass + "\"");
}

WeaponStats2 WeaponController::buildProjectileWeaponStats(
    const WeaponDefinition& definition, 
    const Projectile::Blueprint& projectileBlueprint,
    const DamageSource& damageSource,
    Item* item
) {
    WeaponStats2 stats;
    stats.range = definition.range;
    stats.uses = definition.uses;

    return stats;
}

WeaponStats2 WeaponController::buildMeleeWeaponStats(
    const WeaponDefinition& definition, 
    const DamageSource& damageSource,
    Item* item
) {
    WeaponStats2 stats;
    stats.uses = definition.uses;

    return stats;
}

// TODO: Remove this when we swap over weapon stats
void WeaponController::synchronizeWithItemStats(Item* item, AllStats& weaponStats) {
    // if(item != nullptr) {
    //     item->addStats(weaponStats);
    //     weaponStats.common = item->getStats().common;
    // }
}

std::unique_ptr<Weapon> WeaponController::createWeapon(const std::string& name, Entity* owner) {
    game_assert(initialised);
    return createWeapon(UUID::getNewUUID(), name, owner);
}

Item* WeaponController::getItem(const std::string& itemName, Entity* owner) {
    if(itemName == "") {
        return nullptr;
    }

    auto item = context->getItemController()->addItem(itemName, glm::ivec2(0, 0), owner, false);
    item->setParticipantId(owner->getParticipantId());

    return item;
}