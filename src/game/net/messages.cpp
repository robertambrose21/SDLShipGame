#include "messages.h"
#include "game/weapons/weapon.h"
#include "game/weapons/projectileweapon.h"

WeaponStateUpdate WeaponStateUpdate::serialize(std::shared_ptr<Weapon> weapon) {
    WeaponStateUpdate weaponUpdate;

    switch(weapon->getType()) {
        case Weapon::Type::MELEE:
            strcpy(weaponUpdate.weaponClass, "Melee");
            break;

        case Weapon::Type::PROJECTILE:
            strcpy(weaponUpdate.weaponClass, "Projectile");
            strcpy(weaponUpdate.projectile, std::dynamic_pointer_cast<ProjectileWeapon>(weapon)->getProjectileBluePrint().name.c_str());
            break;

        default:
            break;
    }

    weaponUpdate.id = weapon->getId();
    strcpy(weaponUpdate.name, weapon->getName().c_str());
    weaponUpdate.damage = weapon->getStats().damage;
    weaponUpdate.range = weapon->getStats().range;
    weaponUpdate.uses = weapon->getStats().uses;
    weaponUpdate.usesLeft = weapon->getUsesLeft();

    return weaponUpdate;
}

std::shared_ptr<Weapon> WeaponStateUpdate::deserialize(WeaponStateUpdate update, std::shared_ptr<Weapon> existing) {
    game_assert(existing != nullptr);
    existing->setUsesLeft(update.usesLeft);
    return existing;
}

EntityStateUpdate EntityStateUpdate::serialize(std::shared_ptr<Entity> entity) {
    game_assert(entity != nullptr);

    EntityStateUpdate entityStateUpdate;

    entityStateUpdate.id = entity->getId();
    strcpy(entityStateUpdate.name, entity->getName().c_str());
    entityStateUpdate.totalHP = entity->getStats().hp;
    entityStateUpdate.currentHP = entity->getCurrentHP();
    entityStateUpdate.x = entity->getPosition().x;
    entityStateUpdate.y = entity->getPosition().y;
    entityStateUpdate.movesPerTurn = entity->getStats().movesPerTurn;
    entityStateUpdate.movesLeft = entity->getMovesLeft();
    entityStateUpdate.participantId = entity->getParticipantId();
    entityStateUpdate.currentWeaponId = entity->getCurrentWeapon()->getId();
    entityStateUpdate.numWeapons = entity->getWeapons().size();

    int index = 0;
    for(auto [_, weapon] : entity->getWeapons()) {
        entityStateUpdate.weaponUpdates[index++] = WeaponStateUpdate::serialize(weapon);
    }

    return entityStateUpdate;
}

std::shared_ptr<Entity> EntityStateUpdate::deserialize(EntityStateUpdate update, std::shared_ptr<Entity> existing) {
    game_assert(existing != nullptr);
    existing->setPosition(glm::ivec2(update.x, update.y));
    existing->setCurrentHP(update.currentHP);
    existing->setMovesLeft(update.movesLeft);
    existing->setParticipantId(update.participantId);

    for(int i = 0; i < update.numWeapons; i++) {
        auto& weaponUpdate = update.weaponUpdates[i];
        auto weapon = existing->getWeapon(weaponUpdate.id);

        if(weaponUpdate.id == update.currentWeaponId) {
            existing->setCurrentWeapon(weapon);
        }

        weapon = WeaponStateUpdate::deserialize(weaponUpdate, weapon);
    }

    return existing;
}