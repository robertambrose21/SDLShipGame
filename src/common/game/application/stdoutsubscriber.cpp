#include "stdoutsubscriber.h"

template<typename... Args>
void log(time_t timestamp, std::format_string<Args...> fmt, Args&&... args) {
    std::cout 
        << std::put_time(std::localtime(&timestamp), "[%H:%M:%S]: ")
        << std::vformat(fmt.get(), std::make_format_args(args...))
        << std::endl;
}

StdOutSubscriber::StdOutSubscriber()
{ }

void StdOutSubscriber::onPublish(const Event<TurnEventData>& event) {
    log(event.timestamp, "Turn {}", event.data.turnNumber);
}

void StdOutSubscriber::onPublish(const Event<EntityEventData>& event) {
    if(event.data.type == "Death") {
        log(event.timestamp, "{} died.",getEntityIdentifier(event.data.entity));
    }
    else if(event.data.type == "Freeze" && event.data.entity->getFrozenFor() <= 0) {
        log(event.timestamp, "{} unfreezes.", getEntityIdentifier(event.data.entity));
    }
}

void StdOutSubscriber::onPublish(const Event<WeaponEventData>& event) {
    if(event.data.weapon->getType() != Weapon::Type::MELEE) {
        return;
    }

    log(
        event.timestamp,
        "{} meleed {} for {} damage! {} now has {} HP.",
        getEntityIdentifier(event.data.owner),
        getEntityIdentifier(event.data.target),
        event.data.weapon->getStats().damage,
        getEntityIdentifier(event.data.target),
        event.data.target->getCurrentHP()
    );
}

void StdOutSubscriber::onPublish(const Event<ProjectileEventData>& event) {
    if(event.data.target == nullptr) {
        return;
    }

    if(event.data.damage > 0) {      
        log(
            event.timestamp,
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
        if(effect.name == "freeze") {
            log(event.timestamp, "{} is frozen for {} turns.", getEntityIdentifier(event.data.target), effect.duration);
        }
    }
}

void StdOutSubscriber::onPublish(const Event<AreaOfEffectEventData>& event) {
    log(
        event.timestamp,
        "{} was hit by an area of effect from participant [{}] and took {} damage! {} nopw has {} HP.",
        getEntityIdentifier(event.data.target),
        event.data.aoe->getOwnerId(),
        event.data.aoe->getStats().damagePerTurn,
        getEntityIdentifier(event.data.target),
        event.data.target->getCurrentHP()
    );
}

void StdOutSubscriber::onPublish(const Event<ItemEventData>& event) {
    if(event.data.type == ItemEventData::Type::REMOVED) {
        return;
    }

    if(event.data.owner == nullptr) {
        return;
    }

    std::string items = "";

    for(int i = 0; i < event.data.items.size(); i++) {
        items += event.data.items[i]->getName();

        if(i < event.data.items.size() - 1) {
            items += ", ";
        }
    }

    log(event.timestamp, "{} dropped [{}]", getEntityIdentifier(event.data.owner), items);
}

void StdOutSubscriber::onPublish(const Event<TakeItemActionEventData>& event) {
    std::string items = "";

    for(int i = 0; i < event.data.items.size(); i++) {
        items += event.data.items[i]->getName();

        if(i < event.data.items.size() - 1) {
            items += ", ";
        }
    }

    log(event.timestamp, "{} picked up items: [{}]", getEntityIdentifier(event.data.entity), items);
}

std::string StdOutSubscriber::getEntityIdentifier(Entity* entity) {
    game_assert(entity != nullptr);
    return entity->getName() + "#" + std::to_string(entity->getId());
}