#pragma once

#include <vector>
#include <set>
#include <limits>
#include <optional>

#include "spdlog/spdlog.h"
#include "core/components/factioned.h"

class Entity;
class BehaviourStrategy;
class Engagement;
class Item;

class Participant : public Factioned {
public:
    Participant(int id);

    float distanceToOtherParticipant(Participant* other);

    bool hasEngagement(Participant* other);
    bool hasAnyEngagement(void);
    Engagement* getEngagement(void);
    void engage(Engagement* engagement);
    void disengage(void);
    float getAverageEntitySpeed(void);

    void endTurn(void);
    void passTurn(void);
    void nextTurn(void);

    int getId(void) const;

    bool getIsReady(void) const;
    void setIsReady(bool isReady);
    bool getIsPlayer(void) const;
    void setIsPlayer(bool isPlayer);

    bool isPassingNextTurn(void);

    const std::vector<Entity*>& getEntities(void) const;
    void addEntity(Entity* entity);
    void addEntities(const std::vector<Entity*>& entities);
    void removeEntity(Entity* entity);

    const std::vector<Item*>& getItems(void) const;
    void addItem(Item* item);
    void removeItem(Item* item);

    BehaviourStrategy* getBehaviourStrategy(void);
    void setBehaviourStrategy(std::unique_ptr<BehaviourStrategy> behaviourStrategy);

    void setVisibleEntities(const std::set<Entity*>& visibleEntities);
    const std::set<Entity*>& getVisibleEntities(void) const;
    
    void addVisibleEntity(Entity* entity);
    void removeVisibleEntity(Entity* entity);
    bool hasVisibleEntity(Entity* entity);

private:
    int id;
    bool isReady;
    bool isPlayer;
    std::vector<Entity*> entities;
    std::vector<Item*> items;
    bool passNextTurn;
    std::unique_ptr<BehaviourStrategy> behaviourStrategy;
    
    Engagement* engagement;

    std::set<Entity*> visibleEntities;
};
