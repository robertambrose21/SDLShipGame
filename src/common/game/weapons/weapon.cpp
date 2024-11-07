#include "weapon.h"

Weapon::Weapon(
    Entity* owner,
    ApplicationContext* context,
    Item* item,
    EventPublisher<MeleeWeaponEventData>& publisher,
    const UUID& id,
    const std::string& name,
    const DamageSource& damageSource,
    const WeaponStats2& stats
) :
    id(id),
    owner(owner),
    context(context),
    item(item),
    publisher(publisher),
    name(name),
    damageSource(damageSource),
    stats(stats),
    usesLeft(0)
{
    game_assert(owner != nullptr);
}

Weapon::Weapon(
    Entity* owner,
    ApplicationContext* context,
    Item* item,
    EventPublisher<MeleeWeaponEventData>& publisher,
    const std::string& name,
    const DamageSource& damageSource,
    const WeaponStats2& stats
) :
    Weapon(owner, context, item, publisher, UUID::getNewUUID(), name, damageSource, stats)
{ }

void Weapon::use(const glm::ivec2& position, const glm::ivec2& target, bool isAnimationOnly) {
    if(usesLeft <= 0) {
        return;
    }

    if(!onUse(position, target, isAnimationOnly)) {
        return;
    }
    
    usesLeft--;
}

bool Weapon::hasFinished(void) {
    return usesLeft <= 0;
}

bool Weapon::isAnimationInProgress(void) {
    return false;
}

void Weapon::setFinished(void) {
    usesLeft = 0;
}

void Weapon::reset(void) {
    usesLeft = stats.uses;
}

bool Weapon::isInRange(const glm::ivec2& position) {
    return glm::distance(glm::vec2(owner->getPosition()), glm::vec2(position)) <= stats.range;
}

WeaponStats2 Weapon::getStats(void) const {
    return stats;
}

int Weapon::getUsesLeft(void) const {
    return usesLeft;
}

void Weapon::setUsesLeft(int usesLeft) {
    this->usesLeft = usesLeft;
}

UUID Weapon::getId(void) const {
    return id;
}

std::string Weapon::getName(void) const {
    return name;
}

Entity* Weapon::getOwner(void) {
    return owner;
}

DamageSource Weapon::getDamageSource(void) const {
    return damageSource;
}

Item* Weapon::getItem(void) {
    return item;
}