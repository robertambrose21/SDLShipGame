#include "participant.h"
#include "core/grid/grid.h"
#include "game/actors/actor.h"
#include "game/actors/behaviour/behaviourstrategy.h"
#include "game/items/item.h"

Participant::Participant(int id, uint32_t factionId) :
    Factioned(factionId),
    id(id),
    passNextTurn(false),
    engagement(nullptr)
{ }

// TODO: There's some efficiencies we can do here.
// - Grid based partitioning,
// - k-d Tree? Whatever the hell that is: https://en.wikipedia.org/wiki/K-d_tree
float Participant::distanceToOtherParticipant(Participant* other) {
    game_assert(other != nullptr);
    float shortestDistance = std::numeric_limits<float>::infinity();

    for(auto actor : actors) {
        for(auto otherActor : other->getActors()) {
            float distance = glm::distance(glm::vec2(actor->getPosition()), glm::vec2(otherActor->getPosition()));

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

    for(auto actor : actors) {
        actor->engage();
    }
}

void Participant::disengage(void) {
    engagement = nullptr;

    for(auto actor : actors) {
        actor->disengage();
    }
}

float Participant::getAverageActorSpeed(void) {
    float totalSpeed = 0;

    for(auto actor : actors) {
        totalSpeed += actor->getSpeed();
    }

    return totalSpeed / (float) actors.size();
}

void Participant::endTurn(void) {
    for(auto const& actor : actors) {
        actor->endTurn();
    }

    passNextTurn = false;
}

void Participant::passTurn(void) {
    passNextTurn = true;
}

void Participant::nextTurn(void) {
    std::set<Actor*> actorsForDeletion;

    for(auto const& actor : actors) {
        actor->nextTurn();

        if(actor->getCurrentHP() <= 0) {
            actorsForDeletion.insert(actor);
        }
    }

    for(auto const& actor : actorsForDeletion) {
        actors.erase(std::find(actors.begin(), actors.end(), actor));
    }
    
    actorsForDeletion.clear();

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

const std::vector<Actor*>& Participant::getActors(void) const {
    return actors;
}

void Participant::addActor(Actor* actor) {
    game_assert(!actor->hasParticipant());

    actor->setParticipantId(id);

    if(hasAnyEngagement()) {
        actor->engage();
    }
    
    actors.push_back(actor);
    addVisibleActor(actor);
}

void Participant::addActors(const std::vector<Actor*>& actors) {
    for(auto actor : actors) {
        addActor(actor);
    }
}

void Participant::removeActor(Actor* actor) {
    actors.erase(std::remove(actors.begin(), actors.end(), actor), actors.end());
    actor->setParticipantId(-1);
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

void Participant::setVisibleActors(const std::set<Actor*>& visibleActors) {
    this->visibleActors = visibleActors;
}

const std::set<Actor*>& Participant::getVisibleActors(void) const {
    return visibleActors;
}

void Participant::addVisibleActor(Actor* actor) {
    visibleActors.insert(actor);
}

void Participant::removeVisibleActor(Actor* actor) {
    visibleActors.erase(actor);
}

bool Participant::hasVisibleActor(Actor* actor) {
    return visibleActors.contains(actor);
}

