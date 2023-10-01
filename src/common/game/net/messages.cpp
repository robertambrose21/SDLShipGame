#include "messages.h"
#include "game/weapons/weapon.h"
#include "game/weapons/projectileweapon.h"

WeaponStateUpdate WeaponStateUpdate::serialize(Weapon* weapon) {
    WeaponStateUpdate weaponUpdate;

    switch(weapon->getType()) {
        case Weapon::Type::MELEE:
            strcpy(weaponUpdate.weaponClass, "Melee");
            break;

        case Weapon::Type::PROJECTILE:
            strcpy(weaponUpdate.weaponClass, "Projectile");
            strcpy(weaponUpdate.projectile, ((ProjectileWeapon*) weapon)->getProjectileBluePrint().name.c_str());
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

Weapon* WeaponStateUpdate::deserialize(const WeaponStateUpdate& update, Weapon* existing) {
    game_assert(existing != nullptr);
    existing->setUsesLeft(update.usesLeft);
    return existing;
}

EntityStateUpdate EntityStateUpdate::serialize(Entity* entity) {
    game_assert(entity != nullptr);

    EntityStateUpdate entityStateUpdate;

    entityStateUpdate.id = entity->getId();
    strcpy(entityStateUpdate.name, entity->getName().c_str());
    entityStateUpdate.totalHP = entity->getBaseStats().totalHP;
    entityStateUpdate.currentHP = entity->getCurrentHP();
    entityStateUpdate.x = entity->getPosition().x;
    entityStateUpdate.y = entity->getPosition().y;
    entityStateUpdate.movesPerTurn = entity->getBaseStats().movesPerTurn;
    entityStateUpdate.movesLeft = entity->getMovesLeft();
    entityStateUpdate.participantId = entity->getParticipantId();
    entityStateUpdate.currentWeaponId = entity->getCurrentWeapon()->getId();
    entityStateUpdate.numWeapons = entity->getWeapons().size();

    int index = 0;
    for(auto weapon : entity->getWeapons()) {
        entityStateUpdate.weaponUpdates[index++] = WeaponStateUpdate::serialize(weapon);
    }

    return entityStateUpdate;
}

void EntityStateUpdate::deserialize(const EntityStateUpdate& update, Entity* existing) {
    game_assert(existing != nullptr);
    existing->setPosition(glm::ivec2(update.x, update.y));
    existing->setCurrentHP(update.currentHP);
    existing->setMovesLeft(update.movesLeft);
    existing->setParticipantId(update.participantId);

    for(int i = 0; i < update.numWeapons; i++) {
        auto& weaponUpdate = update.weaponUpdates[i];
        auto weapon = existing->getWeapon(weaponUpdate.id);

        game_assert(weapon != nullptr);

        if(weaponUpdate.id == update.currentWeaponId) {
            existing->setCurrentWeapon(weapon->getId());
        }

        WeaponStateUpdate::deserialize(weaponUpdate, weapon);
    }
}