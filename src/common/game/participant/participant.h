#pragma once

#include <vector>
#include <set>
#include <limits>
#include <optional>

#include "spdlog/spdlog.h"
#include "core/components/factioned.h"

class Actor;
class BehaviourStrategy;
class Engagement;
class Item;

class Participant : public Factioned {
public:
    Participant(int id, uint32_t factionId);

    float distanceToOtherParticipant(Participant* other);

    bool hasEngagement(Participant* other);
    bool hasAnyEngagement(void);
    Engagement* getEngagement(void);
    void engage(Engagement* engagement);
    void disengage(void);
    float getAverageActorSpeed(void);

    void endTurn(void);
    void passTurn(void);
    void nextTurn(void);

    int getId(void) const;

    bool getIsReady(void) const;
    void setIsReady(bool isReady);
    bool getIsPlayer(void) const;
    void setIsPlayer(bool isPlayer);

    bool isPassingNextTurn(void);

    const std::vector<Actor*>& getActors(void) const;
    void addActor(Actor* actor);
    void addActors(const std::vector<Actor*>& actors);
    void removeActor(Actor* actor);

    const std::vector<Item*>& getItems(void) const;
    void addItem(Item* item);
    void removeItem(Item* item);

    BehaviourStrategy* getBehaviourStrategy(void);
    void setBehaviourStrategy(std::unique_ptr<BehaviourStrategy> behaviourStrategy);

    void setVisibleActors(const std::set<Actor*>& visibleActors);
    const std::set<Actor*>& getVisibleActors(void) const;
    
    void addVisibleActor(Actor* actor);
    void removeVisibleActor(Actor* actor);
    bool hasVisibleActor(Actor* actor);

private:
    int id;
    bool isReady;
    bool isPlayer;
    std::vector<Actor*> actors;
    std::vector<Item*> items;
    bool passNextTurn;
    std::unique_ptr<BehaviourStrategy> behaviourStrategy;
    
    Engagement* engagement;

    std::set<Actor*> visibleActors;
};
