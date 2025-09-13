#include "stdoutsubscriber.h"

StdOutSubscriber::StdOutSubscriber(ApplicationContext& context) :
    context(context)
{ }

void StdOutSubscriber::onPublish(const Event<ActorEventData>& event) {
    if(event.data.type == "Death") {
        spdlog::info("{} died.", getActorIdentifier(event.data.entity));
    }
    else if(event.data.type == "Freeze") {
        if(!context.getEntityRegistry().get<Actor>(event.data.entity).getIsFrozen()) {
            return;
        }

        spdlog::info("{} unfreezes.", getActorIdentifier(event.data.entity));
    }
}

void StdOutSubscriber::onPublish(const Event<MeleeWeaponEventData>& event) {
    if(event.data.weapon->getType() != Stats::WeaponStats::MELEE) {
        return;
    }

    auto targetCurrentHP = context.getEntityRegistry().get<Stats::ActorStats>(event.data.target).hp;
    auto participantId = context.getEntityRegistry().get<Actor>(event.data.owner).getParticipantId();
    
    spdlog::info(
        "{} was meleed by participant [{}] and took {} damage! {} now has {} HP.",
        getActorIdentifier(event.data.target),
        participantId,
        event.data.damage,
        getActorIdentifier(event.data.target),
        targetCurrentHP
    );
}

void StdOutSubscriber::onPublish(const Event<ProjectileEventData>& event) {
    if(!event.data.target.has_value()) {
        return;
    }

    auto target = event.data.target.value();
    auto targetCurrentHP = context.getEntityRegistry().get<Stats::ActorStats>(target).hp;

    if(event.data.damage > 0) {      
        spdlog::info(
            "{} was hit by a projectile from participant [{}] and took {} damage! {} now has {} HP.",
            getActorIdentifier(target),
            event.data.projectile->getOwnerId(),
            event.data.damage,
            getActorIdentifier(target),
            targetCurrentHP
        );
    }

    auto effects = event.data.projectile->getStats().effects;
    for(auto effect : effects) {
        if(effect.type == EffectType::FREEZE) {
            spdlog::info("{} is frozen for {} turns.", getActorIdentifier(target), effect.duration);
        }
        else if(effect.type == EffectType::POISON) {
            spdlog::info("{} is poisoned for {} turns.", getActorIdentifier(target), effect.duration);
        }
    }
}

void StdOutSubscriber::onPublish(const Event<AreaOfEffectEventData>& event) {
    auto targetCurrentHP = context.getEntityRegistry().get<Stats::ActorStats>(event.data.target).hp;

    spdlog::info(
        "{} was hit by an area of effect from participant [{}] and took {} damage! {} now has {} HP.",
        getActorIdentifier(event.data.target),
        event.data.aoe->getOwnerId(),
        event.data.damage,
        getActorIdentifier(event.data.target),
        targetCurrentHP
    );
}

void StdOutSubscriber::onPublish(const Event<ItemEventData>& event) {
    if(event.data.type == ItemEventData::Type::REMOVED) {
        return;
    }

    std::string items = "";

    for(int i = 0; i < event.data.items.size(); i++) {
        items += event.data.items[i]->getName();

        if(i < event.data.items.size() - 1) {
            items += ", ";
        }
    }

    if(event.data.droppedBy != Item::UnknownOwner) {
        spdlog::info("{} dropped [{}]", event.data.droppedBy, items);
    }
    else {
        spdlog::info("[{}] was dropped", items);
    }
}

void StdOutSubscriber::onPublish(const Event<TakeItemActionEventData>& event) {
    std::string items = "";

    for(int i = 0; i < event.data.items.size(); i++) {
        items += event.data.items[i]->getName();

        if(i < event.data.items.size() - 1) {
            items += ", ";
        }
    }

    spdlog::info("{} picked up items: [{}]", getActorIdentifier(event.data.entity), items);
}

void StdOutSubscriber::onPublish(const Event<EquipItemActionEventData>& event) {
    if(event.data.isUnequip) {
        spdlog::info(
            "{} unequipped [{}]",
            getActorIdentifier(event.data.entity),
            event.data.item->getName()
        );
    }
    else {
        spdlog::info(
            "{} equipped [{}]",
            getActorIdentifier(event.data.entity),
            event.data.item->getName()
        );
    }
}

void StdOutSubscriber::onPublish(const Event<ApplyDamageEventData>& event) {
    auto targetCurrentHP = context.getEntityRegistry().get<Stats::ActorStats>(event.data.target).hp;

    switch(event.data.source) {
        case DamageType::AOE:
            spdlog::info(
                "{} was hit by an area of effect from participant [{}] and took {} damage! {} now has {} HP.",
                getActorIdentifier(event.data.target),
                event.data.participantId,
                event.data.damage,
                getActorIdentifier(event.data.target),
                targetCurrentHP
            );
            break;

        case DamageType::PROJECTILE:
            spdlog::info(
                "{} was hit by a projectile from participant [{}] and took {} damage! {} now has {} HP.",
                getActorIdentifier(event.data.target),
                event.data.participantId,
                event.data.damage,
                getActorIdentifier(event.data.target),
                targetCurrentHP
            );
            break;

        case DamageType::MELEE:
            spdlog::info(
                "{} was meleed by participant [{}] and took {} damage! {} now has {} HP.",
                getActorIdentifier(event.data.target),
                event.data.participantId,
                event.data.damage,
                getActorIdentifier(event.data.target),
                targetCurrentHP
            );
            break;
        
        default:
            break;
    }
}

std::string StdOutSubscriber::getActorIdentifier(entt::entity entity) {
    auto& actor = context.getEntityRegistry().get<Actor>(entity);
    return actor.getName() + "#" + std::to_string(actor.getId());
}