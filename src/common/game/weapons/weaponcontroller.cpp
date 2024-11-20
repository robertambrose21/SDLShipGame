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

        auto stats = data["stats"].get<json>();
        
        definition.damageSource = stats["damage"].get<std::string>();
        definition.range = stats["range"].get<int>();
        definition.uses = stats["uses"].get<int>();
        definition.power = stats["power"].get<int>();

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

Stats::WeaponStats WeaponController::buildProjectileWeaponStats(
    const WeaponDefinition& definition, 
    const Projectile::Blueprint& projectileBlueprint,
    const DamageSource& damageSource,
    Item* item
) {
    Stats::WeaponStats stats;
    stats.weaponClass = Stats::WeaponStats::PROJECTILE;
    stats.range = definition.range;
    stats.uses = definition.uses;
    stats.damage = damageSource.getStats();
    stats.projectile = projectileBlueprint.stats;

    synchronizeWithItemStats(item, stats);

    return stats;
}

Stats::WeaponStats WeaponController::buildMeleeWeaponStats(
    const WeaponDefinition& definition, 
    const DamageSource& damageSource,
    Item* item
) {
    Stats::WeaponStats stats;
    stats.weaponClass = Stats::WeaponStats::MELEE;
    stats.range = definition.range;
    stats.uses = definition.uses;
    stats.damage = damageSource.getStats();

    synchronizeWithItemStats(item, stats);

    return stats;
}

void WeaponController::synchronizeWithItemStats(Item* item, Stats::WeaponStats& weaponStats) {
    if(item == nullptr) {
        return;
    }

    item->setWeaponStats(weaponStats);
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