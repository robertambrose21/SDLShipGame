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

    strcpy(weaponUpdate.idBytes, weapon->getId().getBytes().data());
    strcpy(weaponUpdate.name, weapon->getName().c_str());
    // weaponUpdate.damage = weapon->getStats().damage;
    weaponUpdate.range = weapon->getStats().range;
    weaponUpdate.uses = weapon->getStats().uses;
    weaponUpdate.usesLeft = weapon->getUsesLeft();
    weaponUpdate.hasItem = weapon->getItem() != nullptr;
    weaponUpdate.itemId = weaponUpdate.hasItem ? weapon->getItem()->getId() : 0;

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
    entityStateUpdate.totalHP = entity->getBaseStats().hp;
    entityStateUpdate.currentHP = entity->getCurrentHP();
    entityStateUpdate.x = entity->getPosition().x;
    entityStateUpdate.y = entity->getPosition().y;
    entityStateUpdate.participantId = entity->getParticipantId();
    entityStateUpdate.isEngaged = entity->isEngaged();
    strcpy(entityStateUpdate.currentWeaponIdBytes, entity->getCurrentWeapon()->getId().getBytes().data());
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
    existing->setParticipantId(update.participantId);

    // if(update.isEngaged && !existing->isEngaged()) {
    //     existing->engage();
    // } else if(!update.isEngaged && existing->isEngaged()) {
    //     existing->disengage();
    // }

    for(int i = 0; i < update.numWeapons; i++) {
        auto& weaponUpdate = update.weaponUpdates[i];
        auto weaponId = UUID::fromBytes(weaponUpdate.idBytes);
        auto weapon = existing->getWeapon(weaponId);

        game_assert(weapon != nullptr);

        if(weaponId == UUID::fromBytes(update.currentWeaponIdBytes)) {
            existing->setCurrentWeapon(weapon->getId());
        }

        WeaponStateUpdate::deserialize(weaponUpdate, weapon);
    }
}