#include "stdoutsubscriber.h"

StdOutSubscriber::StdOutSubscriber()
{ }

void StdOutSubscriber::onPublish(const Event<TurnEventData>& event) {
    spdlog::info("Turn {}", event.data.turnNumber);
}

void StdOutSubscriber::onPublish(const Event<EntityEventData>& event) {
    if(event.data.type == "Death") {
        spdlog::info("{} died.",getEntityIdentifier(event.data.entity));
    }
    else if(event.data.type == "Freeze" && event.data.entity->getIsFrozen()) {
        spdlog::info("{} unfreezes.", getEntityIdentifier(event.data.entity));
    }
}

void StdOutSubscriber::onPublish(const Event<MeleeWeaponEventData>& event) {
    if(event.data.weapon->getType() != Stats::WeaponStats::MELEE) {
        return;
    }
    
    spdlog::info(
        "{} was meleed by participant [{}] and took {} damage! {} now has {} HP.",
        getEntityIdentifier(event.data.target),
        event.data.owner->getParticipantId(),
        event.data.damage,
        getEntityIdentifier(event.data.target),
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
            getEntityIdentifier(event.data.target),
            event.data.projectile->getOwnerId(),
            event.data.damage,
            getEntityIdentifier(event.data.target),
            event.data.target->getCurrentHP()
        );
    }

    auto effects = event.data.projectile->getStats().effects;
    for(auto effect : effects) {
        if(effect.type == EffectType::FREEZE) {
            spdlog::info("{} is frozen for {} turns.", getEntityIdentifier(event.data.target), effect.duration);
        }
        else if(effect.type == EffectType::POISON) {
            spdlog::info("{} is poisoned for {} turns.", getEntityIdentifier(event.data.target), effect.duration);
        }
    }
}

void StdOutSubscriber::onPublish(const Event<AreaOfEffectEventData>& event) {
    spdlog::info(
        "{} was hit by an area of effect from participant [{}] and took {} damage! {} now has {} HP.",
        getEntityIdentifier(event.data.target),
        event.data.aoe->getOwnerId(),
        event.data.damage,
        getEntityIdentifier(event.data.target),
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

    spdlog::info("{} picked up items: [{}]", getEntityIdentifier(event.data.entity), items);
}

void StdOutSubscriber::onPublish(const Event<EngagementEventData>& event) {
    switch(event.data.type) {
        case EngagementType::ENGAGED:
            spdlog::info("participants [{}, {}] are now engaged in combat", event.data.participantIdA, event.data.participantIdB);
            break;
        
        case EngagementType::DISENGAGED:
            spdlog::info("participants [{}, {}] have disengaged from combat", event.data.participantIdA, event.data.participantIdB);
            break;

        default:
            break;
    }
}

void StdOutSubscriber::onPublish(const Event<EquipItemActionEventData>& event) {
    if(event.data.isUnequip) {
        spdlog::info(
            "{} unequipped [{}]",
            getEntityIdentifier(event.data.entity),
            event.data.item->getName()
        );
    }
    else {
        spdlog::info(
            "{} equipped [{}]",
            getEntityIdentifier(event.data.entity),
            event.data.item->getName()
        );
    }
}

void StdOutSubscriber::onPublish(const Event<ApplyDamageEventData>& event) {
    switch(event.data.source) {
        case DamageType::AOE:
            spdlog::info(
                "{} was hit by an area of effect from participant [{}] and took {} damage! {} now has {} HP.",
                getEntityIdentifier(event.data.target),
                event.data.participantId,
                event.data.damage,
                getEntityIdentifier(event.data.target),
                event.data.target->getCurrentHP()
            );
            break;

        case DamageType::PROJECTILE:
            spdlog::info(
                "{} was hit by a projectile from participant [{}] and took {} damage! {} now has {} HP.",
                getEntityIdentifier(event.data.target),
                event.data.participantId,
                event.data.damage,
                getEntityIdentifier(event.data.target),
                event.data.target->getCurrentHP()
            );
            break;

        case DamageType::MELEE:
            spdlog::info(
                "{} was meleed by participant [{}] and took {} damage! {} now has {} HP.",
                getEntityIdentifier(event.data.target),
                event.data.participantId,
                event.data.damage,
                getEntityIdentifier(event.data.target),
                event.data.target->getCurrentHP()
            );
            break;
        
        default:
            break;
    }
}

std::string StdOutSubscriber::getEntityIdentifier(Entity* entity) {
    game_assert(entity != nullptr);
    return entity->getName() + "#" + std::to_string(entity->getId());
}