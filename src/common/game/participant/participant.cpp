#include "participant.h"

Participant::Participant(int id) :
    id(id),
    passNextTurn(false)
{ }

void Participant::engage(int otherParticipantId) {
    engagements.insert(otherParticipantId);

    if(engagements.size() == 1) {
        for(auto entity : entities) {
            entity->engage();
        }
    }
}

void Participant::disengage(int otherParticipantId) {
    engagements.erase(otherParticipantId);

    if(engagements.empty()) {
        for(auto entity : entities) {
            entity->disengage();
        }
    }
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
    entities.push_back(entity);
}

void Participant::addEntities(const std::vector<Entity*>& entities) {
    for(auto entity : entities) {
        this->entities.push_back(entity);
    }
}

void Participant::removeEntity(Entity* entity) {
    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
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

const std::set<int>& Participant::getEngagements(void) const {
    return engagements;
}