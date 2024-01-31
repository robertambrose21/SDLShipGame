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
        
        definition.damage = data["damage"].get<int>();
        definition.range = data["range"].get<int>();
        definition.uses = data["uses"].get<int>();

        std::cout << "Loaded weapon definition \"" << definition.name << "\"" << std::endl;

        weaponDefinitions[definition.name] = definition;
    }
}

std::unique_ptr<Weapon> WeaponController::createWeapon(
    uint32_t id, 
    const std::string& name, 
    Entity* owner,
    Equipment::Slot slot
) {
    game_assert(initialised);
    game_assert(weaponDefinitions.contains(name));

    auto definition = weaponDefinitions[name];
    auto item = getItem(definition.item, owner);

    if(definition.weaponClass == "Projectile") {
        auto weapon = std::make_unique<ProjectileWeapon>(
            owner,
            context->getGrid(),
            context->getEntityPool(),
            context->getProjectilePool(),
            item,
            slot,
            *this,
            id,
            definition.name,
            Weapon::Stats { definition.damage, definition.range, definition.uses },
            context->getProjectilePool()->create(definition.projectile)
        );
        if(item != nullptr) {
            owner->setEquipment(std::make_unique<Equipment>(item, slot));
        }
        return weapon;
    }
    else if(definition.weaponClass == "Melee") {
        auto weapon = std::make_unique<MeleeWeapon>(
            owner,
            context->getGrid(),
            context->getEntityPool(),
            item,
            slot,
            *this,
            id,
            definition.name,
            Weapon::Stats { definition.damage, definition.range, definition.uses }
        );
        if(item != nullptr) {
            owner->setEquipment(std::make_unique<Equipment>(item, slot));
        }
        return weapon;
    }

    throw std::runtime_error("Could not create weapon of class \"" + definition.weaponClass + "\"");
}

std::unique_ptr<Weapon> WeaponController::createWeapon(const std::string& name, Entity* owner, Equipment::Slot slot) {
    game_assert(initialised);
    return createWeapon(getNewId(), name, owner, slot);
}

Item* WeaponController::getItem(const std::string& itemName, Entity* owner) {
    if(itemName == "") {
        return nullptr;
    }

    auto item = context->getItemController()->addItem(itemName, glm::ivec2(0, 0), owner, false);
    item->setParticipantId(owner->getParticipantId());

    return item;
}