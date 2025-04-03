#include "participant.h"
#include "core/grid/grid.h"
#include "game/entities/entity.h"
#include "game/entities/behaviour/behaviourstrategy.h"
#include "game/items/item.h"

Participant::Participant(int id) :
    id(id),
    passNextTurn(false),
    faction("None"),
    engagement(nullptr)
{ }

// TODO: There's some efficiencies we can do here.
// - Grid based partitioning,
// - k-d Tree? Whatever the hell that is: https://en.wikipedia.org/wiki/K-d_tree
float Participant::distanceToOtherParticipant(Participant* other) {
    game_assert(other != nullptr);
    float shortestDistance = std::numeric_limits<float>::infinity();

    for(auto entity : entities) {
        for(auto otherEntity : other->getEntities()) {
            float distance = glm::distance(glm::vec2(entity->getPosition()), glm::vec2(otherEntity->getPosition()));

            if(distance < shortestDistance) {
                shortestDistance = distance;
            }
        }
    }

    return shortestDistance;
}

bool Participant::hasAnyEngagement(void) {
    return engagement != nullptr;
}

bool Participant::hasEngagement(Participant* other) {
    if(engagement == nullptr || other->getEngagement() == nullptr) {
        return false;
    }

    return engagement->getId() == other->getEngagement()->getId();
}

Engagement* Participant::getEngagement(void) {
    return engagement;
}

void Participant::engage(Engagement* engagement) {
    if(this->engagement != nullptr && this->engagement != engagement) {
        spdlog::info(
            "Overriding participant {}'s existing engagement {} with new engagement {}",
            id,
            this->engagement->getId(),
            engagement->getId()
        );
    }
    if(this->engagement == engagement) {
        spdlog::trace(
            "Participant::engage: participant {} already has engagement {}",
            id,
            this->engagement->getId()
        );
    }

    this->engagement = engagement;

    for(auto entity : entities) {
        entity->engage();
    }
}

void Participant::disengage(void) {
    engagement = nullptr;

    for(auto entity : entities) {
        entity->disengage();
    }
}

float Participant::getAverageEntitySpeed(void) {
    float totalSpeed = 0;

    for(auto entity : entities) {
        totalSpeed += entity->getSpeed();
    }

    return totalSpeed / (float) entities.size();
}

void Participant::endTurn(void) {
    for(auto const& entity : entities) {
        entity->endTurn();
    }

    passNextTurn = false;
}

void Participant::passTurn(void) {
    passNextTurn = true;
}

void Participant::nextTurn(void) {
    std::set<Entity*> entitiesForDeletion;

    for(auto const& entity : entities) {
        entity->nextTurn();

        if(entity->getCurrentHP() <= 0) {
            entitiesForDeletion.insert(entity);
        }
    }

    for(auto const& entity : entitiesForDeletion) {
        entities.erase(std::find(entities.begin(), entities.end(), entity));
    }
    
    entitiesForDeletion.clear();

    if(behaviourStrategy != nullptr) {
        behaviourStrategy->onNextTurn();
    }
}

int Participant::getId(void) const {
    return id;
}

bool Participant::getIsReady(void) const {
    return isReady;
}

void Participant::setIsReady(bool isReady) {
    this->isReady = isReady;
}

bool Participant::getIsPlayer(void) const {
    return isPlayer;
}

void Participant::setIsPlayer(bool isPlayer) {
    this->isPlayer = isPlayer;
}

bool Participant::isPassingNextTurn(void) {
    return passNextTurn;
}

const std::vector<Entity*>& Participant::getEntities(void) const {
    return entities;
}

void Participant::addEntity(Entity* entity) {
    game_assert(!entity->hasParticipant());

    entity->setParticipantId(id);

    if(hasAnyEngagement()) {
        entity->engage();
    }
    
    entities.push_back(entity);
    addVisibleEntity(entity);
}

void Participant::addEntities(const std::vector<Entity*>& entities) {
    for(auto entity : entities) {
        addEntity(entity);
    }
}

void Participant::removeEntity(Entity* entity) {
    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
    entity->setParticipantId(-1);
}

const std::vector<Item*>& Participant::getItems(void) const {
    return items;
}

void Participant::addItem(Item* item) {
    items.push_back(item);
}

void Participant::removeItem(Item* item) {
    std::erase(items, item);
}

BehaviourStrategy* Participant::getBehaviourStrategy(void) {
    return behaviourStrategy.get();
}

void Participant::setBehaviourStrategy(std::unique_ptr<BehaviourStrategy> behaviourStrategy) {
    this->behaviourStrategy = std::move(behaviourStrategy);
}

void Participant::setVisibleEntities(const std::set<Entity*>& visibleEntities) {
    this->visibleEntities = visibleEntities;
}

const std::set<Entity*>& Participant::getVisibleEntities(void) const {
    return visibleEntities;
}

void Participant::addVisibleEntity(Entity* entity) {
    visibleEntities.insert(entity);
}

void Participant::removeVisibleEntity(Entity* entity) {
    visibleEntities.erase(entity);
}

bool Participant::hasVisibleEntity(Entity* entity) {
    return visibleEntities.contains(entity);
}

std::string Participant::getFaction(void) const {
    return faction;
}

void Participant::setFaction(const std::string& faction) {
    this->faction = faction;
}

bool Participant::isHostile(Participant* other) {
    return this != other && hostileFactions.contains(other->getFaction());
}

void Participant::addHostileFaction(const std::string& hostileFaction) {
    hostileFactions.insert(hostileFaction);
}

void Participant::removeHostileFaction(const std::string& hostileFaction) {
    hostileFactions.erase(hostileFaction);
}
