#include "entity.h"
#include "game/application/application.h"

Entity::Entity(
    Grid* grid,
    uint32_t id,
    EventPublisher<EntityEventData>& publisher,
    const std::string& name,
    const AllStats& stats
) :
    id(id),
    publisher(publisher),
    name(name),
    baseStats(stats),
    currentStats(stats),
    grid(grid),
    currentWeapon(nullptr),
    position({ 0, 0 }),
    timeSinceLastMoved(0),
    selected(false),
    engaged(false),
    isFrozen(false),
    isPoisoned(false),
    externalActionsChainNeedsRecalculating(true)
{ }

Entity::Entity(
    Grid* grid,
    EventPublisher<EntityEventData>& publisher,
    const std::string& name,
    const AllStats& stats
) : 
    Entity(grid, getNewId(), publisher, name, stats)
{ }

void Entity::setTextureId(uint32_t textureId) {
    this->textureId = textureId;
}

uint32_t Entity::getTextureId(void) const {
    return textureId;
}

uint32_t Entity::getSelectedTextureId(void) const {
    return selectedTextureId;
}

void Entity::setSelectedTextureId(uint32_t selectedTextureId) {
    game_assert(textureId != selectedTextureId);
    this->selectedTextureId = selectedTextureId;
}

void Entity::setColour(const Colour& colour) {
    this->colour = colour;
}

Entity::Colour Entity::getColour(void) const {
    return colour;
}

void Entity::update(int64_t timeSinceLastFrame, bool& quit) {
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
        setPosition(path.front());
        path.pop_front();
        timeSinceLastMoved = 0;
        
        if(isEngaged()) {
            useMoves(1);
        }
    }
}

void Entity::setSelected(bool selected) {
    this->selected = selected;
}

bool Entity::isSelected(void) const {
    return selected;
}

void Entity::engage(void) {
    if(engaged) {
        return;
    }

    engaged = true;
    clearAllActions();
    reset();
}

void Entity::disengage(void) {
    if(!engaged) {
        return;
    }

    engaged = false;
    clearAllActions();
    reset();
}

bool Entity::isEngaged(void) const {
    return engaged;
}

AllStats Entity::getBaseStats(void) const {
    return baseStats;
}

AllStats Entity::getCurrentStats(void) const {
    return currentStats;
}

void Entity::setEquipment(std::unique_ptr<Equipment> equipmentPiece) {
    auto slot = equipmentPiece->getSlot();
    auto item = equipmentPiece->getItem();

    if(equipment[slot] != nullptr && equipment[slot]->getItem()->getId() == item->getId()) {
        return;
    }

    baseStats.add(item->getStats());
    equipment[slot] = std::move(equipmentPiece);
}

void Entity::removeEquipment(Equipment::Slot slot) {
    baseStats.remove(equipment[slot]->getItem()->getStats());
    equipment[slot] = nullptr;
}

Equipment* Entity::getEquipment(Equipment::Slot slot) {
    if(equipment[slot] == nullptr) {
        return nullptr;
    }

    return equipment[slot].get();
}

const float Entity::getSpeed(void) {
    return 2000.0f / (MOVES_PER_SECOND * baseStats.common.moves);
}

int Entity::getCurrentHP(void) const {
    return currentStats.common.hp;
}

void Entity::setCurrentHP(int hp) {
    currentStats.common.hp = hp;
}

void Entity::takeDamage(int amount) {
    currentStats.common.hp -= amount;
}

void Entity::attack(const glm::ivec2& target, const UUID& weaponId, bool isAnimationOnly) {
    // TODO: free use when not engaged
    weapons[weaponId]->use(position, target, isAnimationOnly);
}

std::vector<Weapon*> Entity::getWeapons(void) const {
    std::vector<Weapon*> vWeapons;
    
    for(auto& [_, weapon] : weapons) {
        if(weapon != nullptr) {
            vWeapons.push_back(weapon.get());
        }
    }

    return vWeapons;
}

Weapon* Entity::getWeapon(const UUID& weaponId) {
    return weapons[weaponId].get();
}

bool Entity::hasWeapon(const UUID& weaponId) {
    return weapons.contains(weaponId);
}

Weapon* Entity::addWeapon(std::unique_ptr<Weapon> weapon) {
    auto id = weapon->getId();
    weapons[id] = std::move(weapon);
    return weapons[id].get();
}

void Entity::removeWeapon(const UUID& weaponId) {
    if(weaponId == currentWeapon->getId()) {
        currentWeapon = nullptr;
    }

    weapons.erase(weaponId);
}

void Entity::removeAllWeapons(void) {
    weapons.clear();
}

void Entity::setCurrentWeapon(const UUID& weaponId) {
    if(!hasWeapon(weaponId)) {
        return;
    }

    currentWeapon = weapons[weaponId].get();
}

Weapon* Entity::getCurrentWeapon(void) {
    if(weapons.empty()) {
        return nullptr;
    }

    if(currentWeapon == nullptr) {
        setCurrentWeapon(weapons.begin()->first);
    }

    return currentWeapon;
}

uint32_t Entity::getId(void) const {
    return id;
}

void Entity::setId(uint32_t id) {
    this->id = id;
}

std::string Entity::getName(void) const {
    return name;
}

void Entity::setName(const std::string& name) {
    this->name = name;
}

glm::ivec2 Entity::getPosition(void) const {
    return position;
}

bool Entity::isOnTile(int x, int y) {
    return position == glm::ivec2(x, y);
}

void Entity::setPosition(const glm::ivec2& position) {
    this->position = position;
}

// TODO: use calculatePath
int Entity::findPath(const glm::ivec2& target, int stopShortSteps) {
    auto path = grid->findPath(getPosition(), target);

    if(path.empty()) {
        return 0;
    }

    // Remove the initial path node which is just the entities current position
    path.pop_front(); 

    if(path.size() >= stopShortSteps) {
        for(auto i = 0; i < stopShortSteps; i++) {
            path.pop_back();
        }
    }

    this->path = path;

    return path.size();
}

std::deque<glm::ivec2> Entity::calculatePath(const glm::ivec2& target, int stopShortSteps) {
    auto path = grid->findPath(getPosition(), target);

    if(path.empty()) {
        return path;
    }

    // Remove the initial path node which is just the entities current position
    path.pop_front(); 

    if(path.size() >= stopShortSteps) {
        for(auto i = 0; i < stopShortSteps; i++) {
            path.pop_back();
        }
    }

    return path;
}

void Entity::setPath(const std::deque<glm::ivec2>& path) {
    this->path = path;
}

bool Entity::hasPath(void) {
    return !path.empty();
}

bool Entity::isNeighbour(Entity* entity) const {
    // TODO: This could be more efficient
    return glm::distance(glm::vec2(getPosition()), glm::vec2(entity->getPosition())) < 2;
}

int Entity::getMovesLeft(void) const {
    return currentStats.common.moves;
}

void Entity::setMovesLeft(int movesLeft) {
    currentStats.common.moves = movesLeft;
}

int Entity::getAggroRange(void) const {
    return 10; // temp hardcoded for now
}

int Entity::getDisengagementRange(void) const {
    return 15; // temp hardcoded for now
}

bool Entity::isTurnInProgress(void) {
    return (getCurrentWeapon() != nullptr && !getCurrentWeapon()->hasFinished()) || getMovesLeft() > 0;
}

bool Entity::hasAnimationsInProgress(void) {
    return getCurrentWeapon() != nullptr && getCurrentWeapon()->isAnimationInProgress();
}

void Entity::useMoves(int numMoves) {
    currentStats.common.moves -= numMoves;
    
    if(currentStats.common.moves <= 0) {
        currentStats.common.moves = 0;
        path.clear();
    }
}

void Entity::nextTurn(void) {
    reset();
}

void Entity::reset(void) {
    currentStats.common.moves = baseStats.common.moves;
    path.clear();

    isFrozen = false;
    isPoisoned = false;

    for(auto& [_, weapon] : weapons) {
        if(weapon != nullptr) {
            weapon->reset();
        }
    }
}

void Entity::endTurn(void) {
    currentStats.common.moves = 0;
    path.clear();

    for(auto& [_, weapon] : weapons) {
        weapon->setUsesLeft(0);
    }
}

void Entity::clearAllActions(void) {
    actionsChain.clear();
    externalActionsChain.clear();
}

bool Entity::queueAction(
    ApplicationContext* context,
    std::unique_ptr<Action> action,
    std::function<void(Action&)> onSuccessfulQueue,
    bool skipValidation
) {
    if(!skipValidation && !action->validate(context)) {
        return false;
    }

    onSuccessfulQueue(*action);

    actionsChain[action->getTurnNumber()].push_back(std::move(action));
    externalActionsChainNeedsRecalculating = true;

    return true;
}

std::deque<Action*>& Entity::getActionsChain(int turnNumber) {
    if(externalActionsChainNeedsRecalculating) {
        recalculateActionsChain();
    }

    return externalActionsChain[turnNumber];
}

void Entity::recalculateActionsChain() {
    externalActionsChain.clear();

    for (auto& [turnNumber, actionChain] : actionsChain) {
        for (auto& action : actionChain) {
            externalActionsChain[turnNumber].push_back(action.get());
        }
    }

    externalActionsChainNeedsRecalculating = false;
}

void Entity::popAction(int currentTurnNumber) {
    actionsChain[currentTurnNumber].pop_front();
    externalActionsChainNeedsRecalculating = true;
}

void Entity::setParticipantId(int participantId) {
    this->participantId = participantId;

    for(auto& [id, weapon] : weapons) {
        if(weapon->getItem() != nullptr) {
            weapon->getItem()->setParticipantId(participantId);
        }
    }
 }

int Entity::getParticipantId(void) const {
    return participantId;
}

bool Entity::getIsFrozen(void) const {
    return isFrozen;
}

void Entity::setFrozen(bool isFrozen) {
    this->isFrozen = isFrozen;
}

bool Entity::getIsPoisoned(void) const {
    return isPoisoned;
}

void Entity::setIsPoisoned(bool isPoisoned) {
    this->isPoisoned = isPoisoned;
}