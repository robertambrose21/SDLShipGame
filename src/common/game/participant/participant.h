#pragma once

#include <vector>
#include <set>

#include "game/entities/entity.h"
#include "game/entities/behaviour/behaviourstrategy.h"
#include "game/items/item.h"

class BehaviourStrategy;

class Participant {
private:
    int id;
    bool isReady;
    bool isPlayer;
    std::vector<Entity*> entities;
    std::vector<Item*> items;
    bool passNextTurn;
    std::unique_ptr<BehaviourStrategy> behaviourStrategy;
    std::set<int> engagements;

public:
    Participant(int id);

    void engage(int otherParticipantId);
    void disengage(int otherParticipantId);

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

    const std::set<int>& getEngagements(void) const;
};
