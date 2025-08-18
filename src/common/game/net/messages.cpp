#include "messages.h"
#include "game/weapons/weapon.h"
#include "game/weapons/projectileweapon.h"

WeaponStateUpdate WeaponStateUpdate::serialize(Weapon* weapon) {
    WeaponStateUpdate weaponUpdate;

    switch(weapon->getType()) {
        case Stats::WeaponStats::MELEE:
            strcpy(weaponUpdate.weaponClass, "Melee");
            break;

        case Stats::WeaponStats::PROJECTILE:
            strcpy(weaponUpdate.weaponClass, "Projectile");
            strcpy(weaponUpdate.projectile, ((ProjectileWeapon*) weapon)->getProjectileBluePrint().name.c_str());
            break;

        default:
            break;
    }

    memcpy(weaponUpdate.idBytes, &weapon->getId().getBytes()[0], 16);
    strcpy(weaponUpdate.name, weapon->getName().c_str());
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

ActorStateUpdate ActorStateUpdate::serialize(Actor* actor) {
    game_assert(actor != nullptr);

    ActorStateUpdate actorStateUpdate;

    actorStateUpdate.id = actor->getId();
    strcpy(actorStateUpdate.name, actor->getName().c_str());
    actorStateUpdate.totalHP = actor->getStats().totalHp;
    actorStateUpdate.currentHP = actor->getCurrentHP();
    actorStateUpdate.x = actor->getPosition().x;
    actorStateUpdate.y = actor->getPosition().y;
    actorStateUpdate.participantId = actor->getParticipantId();
    actorStateUpdate.isEngaged = actor->isEngaged();
    memcpy(actorStateUpdate.currentWeaponIdBytes, &actor->getCurrentWeapon()->getId().getBytes()[0], 16);
    actorStateUpdate.numWeapons = actor->getWeapons().size();

    int index = 0;
    for(auto weapon : actor->getWeapons()) {
        actorStateUpdate.weaponUpdates[index++] = WeaponStateUpdate::serialize(weapon);
    }

    return actorStateUpdate;
}

// TODO: Return the actor
void ActorStateUpdate::deserialize(const ActorStateUpdate& update, Actor* existing) {
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