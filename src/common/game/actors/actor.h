#pragma once

#include <string>
#include <format>

#include "core/glmimport.h"
#include "core/util/idgenerator.h"
#include "game/weapons/weapon.h"
#include "core/util/gameassert.h"
#include "core/event/eventpublisher.h"
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

class Actor {
public:
    const uint32_t MOVES_PER_SECOND = 5;

    Actor(Actor&&) = default;
    Actor& operator=(Actor&&) = default; 

    Actor(
        uint32_t id,
        const std::string& name,
        const Stats::ActorStats& stats
    );

    Actor(
        const std::string& name,
        const Stats::ActorStats& stats
    );

    void setSelected(bool selected);
    bool isSelected(void) const;

    bool isEngaged(void) const;
    void setEngaged(bool engaged);

    Stats::ActorStats getBaseStats(void) const;

    void setGear(std::unique_ptr<Gear> gear);
    void removeGear(Equippable<Stats::GearStats>::Slot slot);
    Gear* getGear(Equippable<Stats::GearStats>::Slot slot);

    // TODO: Move me into AttackAction
    void attack(const glm::ivec2& from, const glm::ivec2& target, const UUID& weaponId, bool isAnimationOnly = false);

    std::vector<Weapon*> getWeapons(void) const;
    const std::map<Equippable<Stats::GearStats>::Slot, std::unique_ptr<Gear>>& getEquippedGear(void) const;

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

    void setPath(const std::deque<glm::ivec2>& path);
    bool hasPath(void);
    glm::ivec2 popPath(void);

    void setTimeSinceLastMoved(int64_t timeSinceLastMoved);
    void incrementTimeSinceLastMoved(int64_t amount);
    int64_t getTimeSinceLastMoved(void) const;

    int getAggroRange(void) const;
    int getDisengagementRange(void) const;
    bool hasAnimationsInProgress(void);

    void setParticipantId(int participantId);
    int getParticipantId(void) const;
    bool hasParticipant(void) const;
    
    bool getIsFrozen(void) const;
    void setFrozen(bool isFrozen);

    bool getIsPoisoned(void) const;
    void setIsPoisoned(bool isPoisoned);

private: 
    uint32_t id;

    bool selected;
    bool engaged;

    std::deque<glm::ivec2> path;
    int64_t timeSinceLastMoved;

    Stats::ActorStats baseStats;
    std::map<Equippable<Stats::GearStats>::Slot, std::unique_ptr<Gear>> equippedGear;

    std::map<UUID, std::unique_ptr<Weapon>> weapons;
    Weapon* currentWeapon;

    std::string name;

    int participantId;

    bool isFrozen;
    bool isPoisoned;
};
