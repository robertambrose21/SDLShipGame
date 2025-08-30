#pragma once

#include <string>
#include <format>

#include "core/glmimport.h"
#include "core/util/idgenerator.h"
#include "game/weapons/weapon.h"
#include "core/util/gameassert.h"
#include "core/event/eventpublisher.h"
#include "core/grid/grid.h"
#include "game/application/applicationcontext.h"
#include "game/effects/effect.h"
#include "game/stats/stats.h"
#include "game/items/gear.h"
#include "game/actions/action.h"

// TODO: Fix with modules?
class Weapon;
class Actor;
class Action;
class Effect;
class ApplicationContext;

struct ActorEventData {
    Actor* actor;
    std::string type;
};

class Actor {
public:
    const uint32_t MOVES_PER_SECOND = 5;

    Actor(Actor&&) = default;
    Actor& operator=(Actor&&) = default; 

    Actor(
        Grid* grid,
        uint32_t id,
        EventPublisher<ActorEventData, ActorSetPositionEventData, ActorUpdateStatsEventData>& publisher,
        const std::string& name,
        const Stats::ActorStats& stats
    );

    Actor(
        Grid* grid,
        EventPublisher<ActorEventData, ActorSetPositionEventData, ActorUpdateStatsEventData>& publisher,
        const std::string& name,
        const Stats::ActorStats& stats
    );

    void setSelected(bool selected);
    bool isSelected(void) const;

    void engage(void);
    void disengage(void);
    bool isEngaged(void) const;

    Stats::ActorStats getStats(void) const;

    void setGear(std::unique_ptr<Gear> gear);
    void removeGear(Equippable<Stats::GearStats>::Slot slot);
    Gear* getGear(Equippable<Stats::GearStats>::Slot slot);

    void applyStats();

    const float getSpeed(void);
    int getCurrentHP(void) const;
    void setCurrentHP(uint32_t hp);
    void takeDamage(uint32_t amount);
    void attack(const glm::ivec2& target, const UUID& weaponId, bool isAnimationOnly = false);

    std::vector<Weapon*> getWeapons(void) const;
    Weapon* getWeapon(const UUID& weaponId);
    bool hasWeapon(const UUID& weaponId);
    Weapon* addWeapon(std::unique_ptr<Weapon> weapon);
    void removeWeapon(const UUID& weaponId);
    void removeAllWeapons(void);
    void setCurrentWeapon(const UUID& weaponId);
    Weapon* getCurrentWeapon(void);
    
    uint32_t getId(void) const;
    void setId(uint32_t id);
    std::string getName(void) const;
    std::string toString(void) const;
    void setName(const std::string& name);

    glm::ivec2 getPosition(void) const;
    bool isOnTile(int x, int y);

    void setPosition(const glm::ivec2& position);
    void setPath(const std::deque<glm::ivec2>& path);
    bool isNeighbour(Actor* actor) const;
    bool hasPath(void);
    glm::ivec2 popPath(void);

    void setTimeSinceLastMoved(int64_t timeSinceLastMoved);
    void incrementTimeSinceLastMoved(int64_t amount);
    int64_t getTimeSinceLastMoved(void) const;

    int getMovesLeft(void) const;
    void setMovesLeft(int movesLeft);
    int getAggroRange(void) const;
    int getDisengagementRange(void) const;
    bool isTurnInProgress(void);
    bool hasAnimationsInProgress(void);
    void useMoves(int numMoves);

    bool queueAction(
        ApplicationContext* context,
        std::unique_ptr<Action> action, 
        std::function<void(Action&)> onSuccessfulQueue, 
        bool skipValidation = false
    );
    std::deque<Action*>& getActionsChain(int turnNumber);
    void recalculateActionsChain();
    void popAction(int currentTurnNumber);

    void setParticipantId(int participantId);
    int getParticipantId(void) const;
    bool hasParticipant(void) const;

    void nextTurn(void);
    void endTurn(void);
    void reset(void);
    
    bool getIsFrozen(void) const;
    void setFrozen(bool isFrozen);

    bool getIsPoisoned(void) const;
    void setIsPoisoned(bool isPoisoned);

private: 
    uint32_t id;

    bool selected;
    bool engaged;

    Grid* grid;
    EventPublisher<ActorEventData, ActorSetPositionEventData, ActorUpdateStatsEventData>& publisher;

    glm::ivec2 position;
    std::deque<glm::ivec2> path;
    int64_t timeSinceLastMoved;

    Stats::ActorStats baseStats;
    Stats::ActorStats stats;
    std::map<Equippable<Stats::GearStats>::Slot, std::unique_ptr<Gear>> equippedGear;

    std::map<UUID, std::unique_ptr<Weapon>> weapons;
    Weapon* currentWeapon;

    std::map<int, std::deque<std::unique_ptr<Action>>> actionsChain;
    std::map<int, std::deque<Action*>> externalActionsChain;
    bool externalActionsChainNeedsRecalculating;

    std::string name;

    int participantId;

    bool isFrozen;
    bool isPoisoned;

    void clearAllActions(void);
};
