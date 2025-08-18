#include "stdoutsubscriber.h"

StdOutSubscriber::StdOutSubscriber()
{ }

void StdOutSubscriber::onPublish(const Event<ActorEventData>& event) {
    if(event.data.type == "Death") {
        spdlog::info("{} died.",getActorIdentifier(event.data.actor));
    }
    else if(event.data.type == "Freeze" && event.data.actor->getIsFrozen()) {
        spdlog::info("{} unfreezes.", getActorIdentifier(event.data.actor));
    }
}

void StdOutSubscriber::onPublish(const Event<MeleeWeaponEventData>& event) {
    if(event.data.weapon->getType() != Stats::WeaponStats::MELEE) {
        return;
    }
    
    spdlog::info(
        "{} was meleed by participant [{}] and took {} damage! {} now has {} HP.",
        getActorIdentifier(event.data.target),
        event.data.owner->getParticipantId(),
        event.data.damage,
        getActorIdentifier(event.data.target),
        event.data.target->getCurrentHP()
    );
}

void StdOutSubscriber::onPublish(const Event<ProjectileEventData>& event) {
    if(event.data.target == nullptr) {
        return;
    }

    if(event.data.damage > 0) {      
        spdlog::info(
            "{} was hit by a projectile from participant [{}] and took {} damage! {} now has {} HP.",
            getActorIdentifier(event.data.target),
            event.data.projectile->getOwnerId(),
            event.data.damage,
            getActorIdentifier(event.data.target),
            event.data.target->getCurrentHP()
        );
    }

    auto effects = event.data.projectile->getStats().effects;
    for(auto effect : effects) {
        if(effect.type == EffectType::FREEZE) {
            spdlog::info("{} is frozen for {} turns.", getActorIdentifier(event.data.target), effect.duration);
        }
        else if(effect.type == EffectType::POISON) {
            spdlog::info("{} is poisoned for {} turns.", getActorIdentifier(event.data.target), effect.duration);
        }
    }
}

void StdOutSubscriber::onPublish(const Event<AreaOfEffectEventData>& event) {
    spdlog::info(
        "{} was hit by an area of effect from participant [{}] and took {} damage! {} now has {} HP.",
        getActorIdentifier(event.data.target),
        event.data.aoe->getOwnerId(),
        event.data.damage,
        getActorIdentifier(event.data.target),
        event.data.target->getCurrentHP()
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

    spdlog::info("{} picked up items: [{}]", getActorIdentifier(event.data.actor), items);
}

void StdOutSubscriber::onPublish(const Event<EquipItemActionEventData>& event) {
    if(event.data.isUnequip) {
        spdlog::info(
            "{} unequipped [{}]",
            getActorIdentifier(event.data.actor),
            event.data.item->getName()
        );
    }
    else {
        spdlog::info(
            "{} equipped [{}]",
            getActorIdentifier(event.data.actor),
            event.data.item->getName()
        );
    }
}

void StdOutSubscriber::onPublish(const Event<ApplyDamageEventData>& event) {
    switch(event.data.source) {
        case DamageType::AOE:
            spdlog::info(
                "{} was hit by an area of effect from participant [{}] and took {} damage! {} now has {} HP.",
                getActorIdentifier(event.data.target),
                event.data.participantId,
                event.data.damage,
                getActorIdentifier(event.data.target),
                event.data.target->getCurrentHP()
            );
            break;

        case DamageType::PROJECTILE:
            spdlog::info(
                "{} was hit by a projectile from participant [{}] and took {} damage! {} now has {} HP.",
                getActorIdentifier(event.data.target),
                event.data.participantId,
                event.data.damage,
                getActorIdentifier(event.data.target),
                event.data.target->getCurrentHP()
            );
            break;

        case DamageType::MELEE:
            spdlog::info(
                "{} was meleed by participant [{}] and took {} damage! {} now has {} HP.",
                getActorIdentifier(event.data.target),
                event.data.participantId,
                event.data.damage,
                getActorIdentifier(event.data.target),
                event.data.target->getCurrentHP()
            );
            break;
        
        default:
            break;
    }
}

std::string StdOutSubscriber::getActorIdentifier(Actor* actor) {
    game_assert(actor != nullptr);
    return actor->getName() + "#" + std::to_string(actor->getId());
}