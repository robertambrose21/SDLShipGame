#include "stdoutsubscriber.h"

StdOutSubscriber::StdOutSubscriber()
{ }

void StdOutSubscriber::onPublish(const Event<TurnEventData>& event) {
    std::cout 
        << std::put_time(std::localtime(&event.timestamp), "[%H:%M:%S]: Turn ")
        << event.data.turnNumber
        << std::endl;
}

void StdOutSubscriber::onPublish(const Event<EntityEventData>& event) {
    auto time = std::put_time(std::localtime(&event.timestamp), "[%H:%M:%S]: ");

    if(event.data.type == "Death") {
        std::cout 
            << time
            << getEntityIdentifier(event.data.entity)
            << " died."
            << std::endl;
    }
    else if(event.data.type == "Freeze" && event.data.entity->getFrozenFor() <= 0) {
        std::cout
            << time
            << getEntityIdentifier(event.data.entity)
            << " unfreezes."
            << std::endl;   
    }
}

void StdOutSubscriber::onPublish(const Event<WeaponEventData>& event) {
    if(event.data.weapon->getType() != Weapon::Type::MELEE) {
        return;
    }

    std::cout 
        << std::put_time(std::localtime(&event.timestamp), "[%H:%M:%S]: ") 
        << getEntityIdentifier(event.data.owner)
        << " meleed " 
        << getEntityIdentifier(event.data.target)
        << " for "
        << event.data.weapon->getStats().damage
        << " damage! "
        << getEntityIdentifier(event.data.target)
        << " now has "
        << event.data.target->getCurrentHP()
        << " HP."
        << std::endl;
}

void StdOutSubscriber::onPublish(const Event<ProjectileEventData>& event) {
    if(event.data.target == nullptr) {
        return;
    }
    
    auto time = std::put_time(std::localtime(&event.timestamp), "[%H:%M:%S]: ");

    if(event.data.damage > 0) {
        std::cout 
            << time
            << getEntityIdentifier(event.data.target)
            << " was hit by a projectile from participant ["
            << event.data.projectile->getOwnerId()
            << "] and took "
            << event.data.damage
            << " damage! "
            << getEntityIdentifier(event.data.target)
            << " now has "
            << event.data.target->getCurrentHP()
            << " HP."
            << std::endl;        
    }

    auto effects = event.data.projectile->getStats().effects;
    for(auto effect : effects) {
        if(effect.name == "freeze") {
            std::cout 
                << time
                << getEntityIdentifier(event.data.target)
                << " is frozen for "
                << effect.duration
                << " turns."
                << std::endl;
        }
    }
}

void StdOutSubscriber::onPublish(const Event<AreaOfEffectEventData>& event) {
    std::cout 
        << std::put_time(std::localtime(&event.timestamp), "[%H:%M:%S]: ")
        << getEntityIdentifier(event.data.target)
        << " was hit by an area of effect from participant ["
        << event.data.aoe->getOwnerId()
        << "] and took "
        << event.data.aoe->getStats().damagePerTurn
        << " damage! "
        << getEntityIdentifier(event.data.target)
        << " now has "
        << event.data.target->getCurrentHP()
        << " HP."
        << std::endl;   
}

std::string StdOutSubscriber::getEntityIdentifier(Entity* entity) {
    game_assert(entity != nullptr);
    return entity->getName() + "#" + std::to_string(entity->getId());
}