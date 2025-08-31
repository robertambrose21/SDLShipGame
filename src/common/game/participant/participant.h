#pragma once

#include <vector>
#include <set>
#include <limits>
#include <optional>
#include <entt/entt.hpp>

#include "spdlog/spdlog.h"
#include "core/ecs/components/factioned.h"

class Actor;
class BehaviourStrategy;
class Engagement;
class Item;
class ApplicationContext;

class Participant : public Factioned {
public:
    Participant(ApplicationContext* context, int id, uint32_t factionId);

    float distanceToOtherParticipant(ApplicationContext* context, Participant* other);

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

    const std::vector<entt::entity>& getActors(void) const;
    void addActor(entt::entity entity);
    void addActors(const std::vector<entt::entity>& actors);
    void removeActor(entt::entity entity);

    const std::vector<Item*>& getItems(void) const;
    void addItem(Item* item);
    void removeItem(Item* item);

    BehaviourStrategy* getBehaviourStrategy(void);
    void setBehaviourStrategy(std::unique_ptr<BehaviourStrategy> behaviourStrategy);

    void setVisibleActors(const std::set<entt::entity>& visibleActors);
    const std::set<entt::entity>& getVisibleActors(void) const;
    
    void addVisibleActor(entt::entity entity);
    void removeVisibleActor(entt::entity entity);
    bool hasVisibleActor(entt::entity entity);

private:
    int id;
    bool isReady;
    bool isPlayer;
    std::vector<entt::entity> actors;
    std::vector<Item*> items;
    bool passNextTurn;
    std::unique_ptr<BehaviourStrategy> behaviourStrategy;
    
    Engagement* engagement;
    ApplicationContext* context;

    std::set<entt::entity> visibleActors;
};
