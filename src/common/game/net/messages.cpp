#include "messages.h"
#include "game/weapons/weapon.h"
#include "game/weapons/projectileweapon.h"
#include "game/actors/actorpool.h"

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

ActorStateUpdate ActorStateUpdate::serialize(ApplicationContext* context, entt::entity entity) {
    ActorStateUpdate actorStateUpdate;

    auto& actor = context->getEntityRegistry().get<Actor>(entity);
    auto const& position = context->getEntityRegistry().get<Position>(entity);
    auto const& stats = context->getEntityRegistry().get<Stats::ActorStats>(entity);

    actorStateUpdate.id = actor.getId();
    strcpy(actorStateUpdate.name, actor.getName().c_str());
    actorStateUpdate.totalHP = stats.totalHp;
    actorStateUpdate.currentHP = stats.hp;
    actorStateUpdate.x = position.x;
    actorStateUpdate.y = position.y;
    actorStateUpdate.participantId = actor.getParticipantId();
    actorStateUpdate.isEngaged = actor.isEngaged();
    memcpy(actorStateUpdate.currentWeaponIdBytes, &actor.getCurrentWeapon()->getId().getBytes()[0], 16);
    actorStateUpdate.numWeapons = actor.getWeapons().size();

    int index = 0;
    for(auto weapon : actor.getWeapons()) {
        actorStateUpdate.weaponUpdates[index++] = WeaponStateUpdate::serialize(weapon);
    }

    return actorStateUpdate;
}

// TODO: Return the actor
void ActorStateUpdate::deserialize(ApplicationContext* context, const ActorStateUpdate& update, entt::entity existing) {
    auto& actor = context->getEntityRegistry().get<Actor>(existing);
    auto& stats = context->getEntityRegistry().get<Stats::ActorStats>(existing);
    context->getEntityRegistry().replace<Position>(existing, glm::ivec2(update.x, update.y));
    context->getEntityRegistry().get_or_emplace<PositionDirty>(existing);

    stats.hp = update.currentHP;
    actor.setParticipantId(update.participantId);

    // if(update.isEngaged && !existing->isEngaged()) {
    //     existing->engage();
    // } else if(!update.isEngaged && existing->isEngaged()) {
    //     existing->disengage();
    // }

    for(int i = 0; i < update.numWeapons; i++) {
        auto& weaponUpdate = update.weaponUpdates[i];
        auto weaponId = UUID::fromBytes(weaponUpdate.idBytes);
        auto weapon = actor.getWeapon(weaponId);

        game_assert(weapon != nullptr);

        if(weaponId == UUID::fromBytes(update.currentWeaponIdBytes)) {
            actor.setCurrentWeapon(weapon->getId());
        }

        WeaponStateUpdate::deserialize(weaponUpdate, weapon);
    }
}

GameStateUpdate GameStateUpdate::serialize(
        ApplicationContext* context,
        int currentParticipantId, 
        const std::vector<Actor*>& actors,
        uint32_t chunkId,
        uint8_t numExpectedChunks
    ) {
        GameStateUpdate update;
        update.numActors = actors.size();

        int index = 0;
        for(auto actor : actors) {
            auto entity = context->getActorPool()->getByExternalId(actor->getId());

            update.actors[index++] = ActorStateUpdate::serialize(context, entity.value());
        }

        update.currentParticipantId = currentParticipantId;
        update.numExpectedChunks = numExpectedChunks;

        return update;
    }