#include "actor.h"
#include "game/application/application.h"

Actor::Actor(
    Grid* grid,
    uint32_t id,
    EventPublisher<ActorEventData, ActorSetPositionEventData, ActorUpdateStatsEventData>& publisher,
    const std::string& name,
    const Stats::ActorStats& stats
) :
    id(id),
    publisher(publisher),
    name(name),
    stats(std::move(stats)),
    baseStats(stats),
    grid(grid),
    currentWeapon(nullptr),
    position({ 0, 0 }),
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
    EventPublisher<ActorEventData, ActorSetPositionEventData, ActorUpdateStatsEventData>& publisher,
    const std::string& name,
    const Stats::ActorStats& stats
) : 
    Actor(grid, getNewId(), publisher, name, stats)
{ }

void Actor::setTextureId(uint32_t textureId) {
    this->textureId = textureId;
}

uint32_t Actor::getTextureId(void) const {
    return textureId;
}

uint32_t Actor::getSelectedTextureId(void) const {
    return selectedTextureId;
}

void Actor::setSelectedTextureId(uint32_t selectedTextureId) {
    game_assert(textureId != selectedTextureId);
    this->selectedTextureId = selectedTextureId;
}

void Actor::setColour(const Colour& colour) {
    this->colour = colour;
}

Actor::Colour Actor::getColour(void) const {
    return colour;
}

void Actor::update(int64_t timeSinceLastFrame, bool& quit) {
    for(auto& [_, weapon] : weapons) {
        weapon->update(timeSinceLastFrame);
    }

    if(isFrozen) {
        return;
    }
    
    if(isEngaged() && getMovesLeft() == 0) {
        return;
    }

    if(path.empty()) {
        return;
    }

    timeSinceLastMoved += timeSinceLastFrame;

    if(timeSinceLastMoved > getSpeed()) {
        auto position = path.front();
        path.pop_front();
        timeSinceLastMoved = 0;

        if(isEngaged()) {
            useMoves(1);
        }

        setPosition(position);
    }
}

void Actor::setSelected(bool selected) {
    this->selected = selected;
}

bool Actor::isSelected(void) const {
    return selected;
}

void Actor::engage(void) {
    if(engaged) {
        return;
    }

    engaged = true;
    clearAllActions();
    reset();
}

void Actor::disengage(void) {
    if(!engaged) {
        return;
    }

    engaged = false;
    clearAllActions();
    reset();
}

bool Actor::isEngaged(void) const {
    return engaged;
}

Stats::ActorStats Actor::getStats(void) const {
    return stats;
}

void Actor::setGear(std::unique_ptr<Gear> gear) {
    if(!gear->isValid()) {
        std::cout << std::format("Warning: invalid gear for slot {}", 
                Equippable<Stats::GearStats>::SLOT_NAMES[gear->getSlot()]) << std::endl;
        return;
    }

    equippedGear[gear->getSlot()] = std::move(gear);
    applyStats();
}

void Actor::removeGear(Equippable<Stats::GearStats>::Slot slot) {
    equippedGear[slot] = nullptr;
    applyStats();
}

Gear* Actor::getGear(Equippable<Stats::GearStats>::Slot slot) {
    return equippedGear[slot].get();
}

void Actor::applyStats() {
    uint32_t currentHp = stats.hp;
    stats = baseStats;
    stats.hp = currentHp;

    for(auto const& [slot, gear] : equippedGear) {
        if(gear != nullptr) {
            gear->addTo(stats);
        }
    }

    for(auto const& [_, weapon] : weapons) {
        if(weapon != nullptr) {
            weapon->addTo(stats);
        }
    }

    publisher.publish<ActorUpdateStatsEventData>({ this });
}

const float Actor::getSpeed(void) {
    return 2000.0f / (MOVES_PER_SECOND * stats.movesPerTurn);
}

int Actor::getCurrentHP(void) const {
    return stats.hp;
}

void Actor::setCurrentHP(uint32_t hp) {
    stats.hp = hp;
    publisher.publish<ActorUpdateStatsEventData>({ this });
}

void Actor::takeDamage(uint32_t amount) {
    stats.hp -= amount;
    publisher.publish<ActorUpdateStatsEventData>({ this });
}

void Actor::attack(const glm::ivec2& target, const UUID& weaponId, bool isAnimationOnly) {
    // TODO: free use when not engaged
    weapons[weaponId]->use(position, target, isAnimationOnly);
    publisher.publish<ActorUpdateStatsEventData>({ this });
}

std::vector<Weapon*> Actor::getWeapons(void) const {
    std::vector<Weapon*> vWeapons;
    
    for(auto& [_, weapon] : weapons) {
        if(weapon != nullptr) {
            vWeapons.push_back(weapon.get());
        }
    }

    return vWeapons;
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

    applyStats();

    return weapons[id].get();
}

void Actor::removeWeapon(const UUID& weaponId) {
    if(weaponId == currentWeapon->getId()) {
        currentWeapon = nullptr;
    }

    weapons.erase(weaponId);
    applyStats();
}

void Actor::removeAllWeapons(void) {
    weapons.clear();
    applyStats();
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

glm::ivec2 Actor::getPosition(void) const {
    return position;
}

bool Actor::isOnTile(int x, int y) {
    return position == glm::ivec2(x, y);
}

void Actor::setPosition(const glm::ivec2& position) {
    this->position = position;
    publisher.publish<ActorSetPositionEventData>({ this, position });
}

// TODO: use calculatePath
int Actor::findPath(const glm::ivec2& target, int stopShortSteps) {
    auto path = grid->findPath(getPosition(), target);

    if(path.empty()) {
        return 0;
    }

    // Remove the initial path node which is just the actors current position
    path.pop_front(); 

    if(path.size() >= stopShortSteps) {
        for(auto i = 0; i < stopShortSteps; i++) {
            path.pop_back();
        }
    }

    this->path = path;

    return path.size();
}

std::deque<glm::ivec2> Actor::calculatePath(const glm::ivec2& target, int stopShortSteps) {
    auto path = grid->findPath(getPosition(), target);

    if(path.empty()) {
        return path;
    }

    // Remove the initial path node which is just the actors current position
    path.pop_front(); 

    if(path.size() >= stopShortSteps) {
        for(auto i = 0; i < stopShortSteps; i++) {
            path.pop_back();
        }
    }

    return path;
}

void Actor::setPath(const std::deque<glm::ivec2>& path) {
    this->path = path;
}

bool Actor::hasPath(void) {
    return !path.empty();
}

bool Actor::isNeighbour(Actor* actor) const {
    // TODO: This could be more efficient
    return glm::distance(glm::vec2(getPosition()), glm::vec2(actor->getPosition())) < 2;
}

int Actor::getMovesLeft(void) const {
    return stats.movesLeft;
}

void Actor::setMovesLeft(int movesLeft) {
    stats.movesLeft = movesLeft;
    publisher.publish<ActorUpdateStatsEventData>({ this });
}

int Actor::getAggroRange(void) const {
    return 10; // temp hardcoded for now
}

int Actor::getDisengagementRange(void) const {
    return 15; // temp hardcoded for now
}

bool Actor::isTurnInProgress(void) {
    return (getCurrentWeapon() != nullptr && !getCurrentWeapon()->hasFinished()) || getMovesLeft() > 0;
}

bool Actor::hasAnimationsInProgress(void) {
    return getCurrentWeapon() != nullptr && getCurrentWeapon()->isAnimationInProgress();
}

void Actor::useMoves(int numMoves) {
    stats.movesLeft -= numMoves;
    
    if(stats.movesLeft <= 0) {
        stats.movesLeft = 0;
        path.clear();
    }

    publisher.publish<ActorUpdateStatsEventData>({ this });
}

void Actor::nextTurn(void) {
    reset();
}

void Actor::reset(void) {
    stats.movesLeft = stats.movesPerTurn;
    path.clear();

    isFrozen = false;
    isPoisoned = false;

    for(auto& [_, weapon] : weapons) {
        if(weapon != nullptr) {
            weapon->reset();
        }
    }

    publisher.publish<ActorUpdateStatsEventData>({ this });
}

void Actor::endTurn(void) {
    stats.movesLeft = 0;
    path.clear();

    for(auto& [_, weapon] : weapons) {
        weapon->setUsesLeft(0);
    }

    publisher.publish<ActorUpdateStatsEventData>({ this });
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