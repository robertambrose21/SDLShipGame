#include "actor.h"
#include "game/application/application.h"

Actor::Actor(
    Grid* grid,
    uint32_t id,
    const std::string& name,
    const Stats::ActorStats& stats
) :
    id(id),
    name(name),
    baseStats(stats),
    grid(grid),
    currentWeapon(nullptr),
    timeSinceLastMoved(0),
    selected(false),
    engaged(false),
    isFrozen(false),
    isPoisoned(false),
    externalActionsChainNeedsRecalculating(true),
    participantId(-1)
{ }

Actor::Actor(
    Grid* grid,
    const std::string& name,
    const Stats::ActorStats& stats
) : 
    Actor(grid, getNewId(), name, stats)
{ }

void Actor::setSelected(bool selected) {
    this->selected = selected;
}

bool Actor::isSelected(void) const {
    return selected;
}

bool Actor::isEngaged(void) const {
    return engaged;
}

void Actor::setEngaged(bool engaged) {
    this->engaged = engaged;
}

Stats::ActorStats Actor::getBaseStats(void) const {
    return baseStats;
}

void Actor::setGear(std::unique_ptr<Gear> gear) {
    if(!gear->isValid()) {
        std::cout << std::format("Warning: invalid gear for slot {}", 
                Equippable<Stats::GearStats>::SLOT_NAMES[gear->getSlot()]) << std::endl;
        return;
    }

    equippedGear[gear->getSlot()] = std::move(gear);
}

void Actor::removeGear(Equippable<Stats::GearStats>::Slot slot) {
    equippedGear[slot] = nullptr;
}

Gear* Actor::getGear(Equippable<Stats::GearStats>::Slot slot) {
    return equippedGear[slot].get();
}

void Actor::attack(const glm::ivec2& from, const glm::ivec2& target, const UUID& weaponId, bool isAnimationOnly) {
    weapons[weaponId]->use(from, target, isAnimationOnly);
}

// TODO: Remove me
std::vector<Weapon*> Actor::getWeapons(void) const {
    std::vector<Weapon*> vWeapons;
    
    for(auto& [_, weapon] : weapons) {
        if(weapon != nullptr) {
            vWeapons.push_back(weapon.get());
        }
    }

    return vWeapons;
}

const std::map<Equippable<Stats::GearStats>::Slot, std::unique_ptr<Gear>>& Actor::getEquippedGear(void) const {
    return equippedGear;
}

Weapon* Actor::getWeapon(const UUID& weaponId) {
    return weapons[weaponId].get();
}

bool Actor::hasWeapon(const UUID& weaponId) {
    return weapons.contains(weaponId);
}

Weapon* Actor::addWeapon(std::unique_ptr<Weapon> weapon) {
    auto id = weapon->getId();
    weapons[id] = std::move(weapon);

    if(weapons.empty()) {
        currentWeapon = weapons[id].get();
    }

    return weapons[id].get();
}

void Actor::removeWeapon(const UUID& weaponId) {
    if(weaponId == currentWeapon->getId()) {
        currentWeapon = nullptr;
    }

    weapons.erase(weaponId);
}

void Actor::removeAllWeapons(void) {
    weapons.clear();
}

void Actor::setCurrentWeapon(const UUID& weaponId) {
    if(!hasWeapon(weaponId)) {
        return;
    }

    currentWeapon = weapons[weaponId].get();
}

Weapon* Actor::getCurrentWeapon(void) {
    if(weapons.empty()) {
        return nullptr;
    }

    if(currentWeapon == nullptr) {
        setCurrentWeapon(weapons.begin()->first);
    }

    return currentWeapon;
}

uint32_t Actor::getId(void) const {
    return id;
}

void Actor::setId(uint32_t id) {
    this->id = id;
}

std::string Actor::getName(void) const {
    return name;
}

std::string Actor::toString(void) const {
    return name + "#" + std::to_string(id);
}

void Actor::setName(const std::string& name) {
    this->name = name;
}

void Actor::setPath(const std::deque<glm::ivec2>& path) {
    this->path = path;
}

bool Actor::hasPath(void) {
    return !path.empty();
}

glm::ivec2 Actor::popPath(void) {
    auto pathFront = path.front();
    path.pop_front();
    return pathFront;
}

void Actor::setTimeSinceLastMoved(int64_t timeSinceLastMoved) {
    this->timeSinceLastMoved = timeSinceLastMoved;
}

void Actor::incrementTimeSinceLastMoved(int64_t amount) {
    timeSinceLastMoved += amount;
}

int64_t Actor::getTimeSinceLastMoved(void) const {
    return timeSinceLastMoved;
}

int Actor::getAggroRange(void) const {
    return 10; // temp hardcoded for now
}

int Actor::getDisengagementRange(void) const {
    return 15; // temp hardcoded for now
}

bool Actor::hasAnimationsInProgress(void) {
    return getCurrentWeapon() != nullptr && getCurrentWeapon()->isAnimationInProgress();
}

void Actor::clearAllActions(void) {
    actionsChain.clear();
    externalActionsChain.clear();
}

bool Actor::queueAction(
    ApplicationContext* context,
    std::unique_ptr<Action> action,
    std::function<void(Action&)> onSuccessfulQueue,
    bool skipValidation
) {
    if(!action->getTurnNumber().has_value()) {
        spdlog::error("[{}], Trying to queue with no turn number, dropping action", action->typeToString());
        return false;
    }

    if(!skipValidation && !action->validate(context)) {
        return false;
    }

    onSuccessfulQueue(*action);

    actionsChain[action->getTurnNumber().value()].push_back(std::move(action));
    externalActionsChainNeedsRecalculating = true;

    return true;
}

std::deque<Action*>& Actor::getActionsChain(int turnNumber) {
    if(externalActionsChainNeedsRecalculating) {
        recalculateActionsChain();
    }

    return externalActionsChain[turnNumber];
}

void Actor::recalculateActionsChain() {
    externalActionsChain.clear();

    for (auto& [turnNumber, actionChain] : actionsChain) {
        for (auto& action : actionChain) {
            externalActionsChain[turnNumber].push_back(action.get());
        }
    }

    externalActionsChainNeedsRecalculating = false;
}

void Actor::popAction(int currentTurnNumber) {
    actionsChain[currentTurnNumber].pop_front();
    externalActionsChainNeedsRecalculating = true;
}

void Actor::setParticipantId(int participantId) {
    this->participantId = participantId;

    for(auto& [id, weapon] : weapons) {
        if(weapon->getItem() != nullptr) {
            weapon->getItem()->setParticipantId(participantId);
        }
    }
 }

int Actor::getParticipantId(void) const {
    return participantId;
}

bool Actor::hasParticipant(void) const {
    return participantId != -1;
}

bool Actor::getIsFrozen(void) const {
    return isFrozen;
}

void Actor::setFrozen(bool isFrozen) {
    this->isFrozen = isFrozen;
}

bool Actor::getIsPoisoned(void) const {
    return isPoisoned;
}

void Actor::setIsPoisoned(bool isPoisoned) {
    this->isPoisoned = isPoisoned;
}