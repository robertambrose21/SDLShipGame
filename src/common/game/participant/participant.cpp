#include "participant.h"

Participant::Participant(int id) :
    id(id),
    passNextTurn(false),
    faction("None")
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

void Participant::engage(int otherParticipantId, int turnEngaged) {
    for(auto engagement : engagements) {
        if(engagement.otherParticipantId == otherParticipantId) {
            spdlog::trace("Attempting to engage to already engaged participant {} -> {}", id, otherParticipantId);
            return;
        }
    }

    engagements.insert({ otherParticipantId, turnEngaged });

    if(engagements.size() == 1) {
        for(auto entity : entities) {
            entity->engage();
        }
    }
}

void Participant::disengage(int otherParticipantId) {
    std::erase_if(engagements, [&](const auto& engagement) {
        return otherParticipantId == engagement.otherParticipantId;
    });

    if(engagements.empty()) {
        for(auto entity : entities) {
            entity->disengage();
        }
    }
}

bool Participant::hasEngagement(int otherParticipantId) {
    for(auto const& engagement : engagements) {
        if(otherParticipantId == engagement.otherParticipantId) {
            return true;
        }
    }

    return false;
}

bool Participant::isEngaged(void) {
    return !engagements.empty();
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

const std::set<Participant::Engagement>& Participant::getEngagements(void) const {
    return engagements;
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
    return hostileFactions.contains(other->getFaction());
}

void Participant::addHostileFaction(const std::string& hostileFaction) {
    hostileFactions.insert(hostileFaction);
}

void Participant::removeHostileFaction(const std::string& hostileFaction) {
    hostileFactions.erase(hostileFaction);
}
