#include "entity.h"
#include "game/application/application.h"

Entity::Entity(
    uint32_t id,
    const std::string& name,
    const EntityBaseStats& stats
) :
    id(id),
    name(name),
    stats(stats),
    currentStats(stats),
    position({ 0, 0 }),
    timeSinceLastMoved(0),
    selected(false),
    frozenForNumTurns(0)
{
    grid = Application::getContext()->getGrid();
    healthBar = std::make_shared<HealthBar>(stats.totalHP);
}

Entity::Entity(
    const std::string& name,
    const EntityBaseStats& stats
) : Entity(getNewId(), name, stats)
{ }

void Entity::setTextureId(uint32_t textureId) {
    this->textureId = textureId;
}

uint32_t Entity::getTextureId(void) const {
    return textureId;
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

void Entity::draw(const std::shared_ptr<GraphicsContext>& graphicsContext) {
    game_assert(graphicsContext != nullptr);

    graphicsContext->getGridRenderer()->draw(
        graphicsContext,
        textureId,
        { colour.r, colour.g, colour.b },
        colour.a,
        position
    );

    if(selected) {
        graphicsContext->getGridRenderer()->draw(graphicsContext, selectedTextureId, position);
    }

    for(auto [_, weapon] : weapons) {
        weapon->draw(graphicsContext);
    }

    if(frozenForNumTurns > 0) {
        auto const &realPosition = graphicsContext->getGridRenderer()->getTilePosition(position.x, position.y);
        auto const &size = graphicsContext->getGridRenderer()->getTileSize();

        SDL_Rect frozen = { realPosition.x, realPosition.y, size, size };

        SDL_SetRenderDrawBlendMode(graphicsContext->getRenderer().get(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(graphicsContext->getRenderer().get(), 0x00, 0xFF, 0xFF, 0x7F);
        SDL_RenderFillRect(graphicsContext->getRenderer().get(), &frozen);
    }

    healthBar->draw(graphicsContext, position, currentStats.totalHP);
}

void Entity::update(uint32_t timeSinceLastFrame, bool& quit) {
    for(auto [_, weapon] : weapons) {
        weapon->update(timeSinceLastFrame);
    }

    if(frozenForNumTurns > 0) {
        return;
    }
    
    if(getMovesLeft() == 0) {
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
        useMoves(1);
    }
}

void Entity::setSelected(bool selected) {
    this->selected = selected;
}

bool Entity::isSelected(void) const {
    return selected;
}

EntityBaseStats Entity::getBaseStats(void) const {
    return stats;
}

EntityCurrentStats Entity::getCurrentStats(void) const {
    return currentStats;
}

const float Entity::getSpeed(void) {
    return 1000.0f / (MOVES_PER_SECOND * getCurrentStats().movesPerTurn);
}

int Entity::getCurrentHP(void) const {
    return currentStats.totalHP;
}

void Entity::setCurrentHP(int hp) {
    currentStats.totalHP = hp;
}

void Entity::takeDamage(int amount) {
    currentStats.totalHP -= amount;
}

void Entity::attack(const std::shared_ptr<Entity>& target, const std::shared_ptr<Weapon>& weapon) {
    auto targetName = target->getName();

    weapon->use(position, target);
}

std::map<uint32_t, std::shared_ptr<Weapon>> Entity::getWeapons(void) const {
    return weapons;
}

std::shared_ptr<Weapon> Entity::getWeapon(uint32_t weaponId) {
    game_assert(weapons.contains(weaponId));
    return weapons[weaponId];
}

bool Entity::hasWeapon(uint32_t weaponId) {
    return weapons.contains(weaponId);
}

std::shared_ptr<Weapon> Entity::addWeapon(const std::shared_ptr<Weapon>& weapon) {
    weapons[weapon->getId()] = weapon;
    return weapon;
}

void Entity::removeWeapon(const std::string& name) {
    for(auto [weaponId, weapon] : weapons) {
        if(weapon->getName() == name) {
            weapons.erase(weaponId);
        }
    }
}

void Entity::setCurrentWeapon(const std::shared_ptr<Weapon>& weapon) {
    currentWeapon = weapon;
}

std::shared_ptr<Weapon> Entity::getCurrentWeapon(void) {
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

int Entity::findPath(const glm::ivec2& target, int stopShortSteps) {
    auto path = grid->findPath(getPosition(), target);

    if(path.empty()) {
        return false;
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

bool Entity::hasPath(void) {
    return !path.empty();
}

bool Entity::isNeighbour(const std::shared_ptr<Entity>& entity) const {
    return glm::distance(glm::vec2(getPosition()), glm::vec2(entity->getPosition())) < 2;
}

int Entity::getMovesLeft(void) const {
    return currentStats.movesLeft;
}

void Entity::setMovesLeft(int movesLeft) {
    currentStats.movesLeft = movesLeft;
}

bool Entity::isTurnInProgress(void) const {
    return getMovesLeft() > 0 || !currentWeapon->hasFinished();
}

void Entity::useMoves(int numMoves) {
    currentStats.movesLeft -= numMoves;
    
    if(currentStats.movesLeft < 0) {
        currentStats.movesLeft = 0;
    }
}

void Entity::nextTurn(void) {
    currentStats.movesLeft = stats.movesPerTurn;
    path.clear();

    for(auto [_, weapon] : weapons) {
        weapon->reset();
    }

    if(frozenForNumTurns > 0) {
        frozenForNumTurns--;

        if(frozenForNumTurns == 0) {
            std::cout << "[" << name << "#" << id << "] unfreezes!" << std::endl;
        }
        else {
            std::cout << "[" << name << "#" << id << "] will be unfrozen in " << frozenForNumTurns << " turns" << std::endl;
        }
    }
}

// TODO: Maybe irrelevant now?
bool Entity::endTurnCondition(void) {
    return false;
}

void Entity::reset(void) {
    currentStats.movesLeft = 0;
    currentStats.movesPerTurn = stats.movesPerTurn;
    currentStats.totalHP = stats.totalHP;
    path.clear();

    for(auto [_, weapon] : weapons) {
        weapon->reset();
    }
}

void Entity::setParticipantId(int participantId) {
    this->participantId = participantId;
}

int Entity::getParticipantId(void) const {
    return participantId;
}

bool Entity::getIsFrozen(void) const {
    return frozenForNumTurns > 0;
}

void Entity::setFrozenFor(int numTurns) {
    frozenForNumTurns = numTurns;
}