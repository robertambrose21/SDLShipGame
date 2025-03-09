#pragma once

#include <vector>
#include <set>
#include <limits>
#include <optional>

#include "spdlog/spdlog.h"

class Entity;
class BehaviourStrategy;
class Engagement;
class Item;

class Participant {
public:
    // typedef struct _enagement {
    //     int otherParticipantId;
    //     int turnEngaged;

    //     // We don't care about the turn engaged
    //     bool operator<(const _enagement& other) const {
    //         return otherParticipantId < other.otherParticipantId;
    //     }
    // } Engagement;

    Participant(int id);

    float distanceToOtherParticipant(Participant* other);

    // void engage(int otherParticipantId, int turnEngaged);
    // void disengage(int otherParticipantId);
    // bool hasEngagement(int otherParticipantId);
    bool hasEngagement(Participant* other);
    bool hasAnyEngagement(void);
    // bool isEngaged(void);
    // std::optional<uint32_t> getEngagementId(void) const;
    // void setEngagementId(const std::optional<uint32_t>& engagementId);
    Engagement* getEngagement(void);
    // void setEngagement(Engagement* engagement);
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

    // const std::set<Engagement>& getEngagements(void) const;

    void setVisibleEntities(const std::set<Entity*>& visibleEntities);
    const std::set<Entity*>& getVisibleEntities(void) const;
    
    void addVisibleEntity(Entity* entity);
    void removeVisibleEntity(Entity* entity);
    bool hasVisibleEntity(Entity* entity);

    std::string getFaction(void) const;
    void setFaction(const std::string& faction);
    bool isHostile(Participant* other);
    void addHostileFaction(const std::string& hostileFaction);
    void removeHostileFaction(const std::string& hostileFaction);

private:
    int id;
    bool isReady;
    bool isPlayer;
    std::string faction;
    std::vector<Entity*> entities;
    std::vector<Item*> items;
    bool passNextTurn;
    std::unique_ptr<BehaviourStrategy> behaviourStrategy;
    
    // std::set<Engagement> engagements;
    // std::optional<uint32_t> engagementId;
    Engagement* engagement;
    std::set<std::string> hostileFactions;

    std::set<Entity*> visibleEntities;
};
