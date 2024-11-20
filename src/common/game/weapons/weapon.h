#pragma once

#include <string>
#include <functional>
#include <memory>

#include "damagesource.h"
#include "game/entities/entity.h"
#include "core/event/eventpublisher.h"
#include "core/grid/grid.h"
#include "game/event/events.h"
#include "game/stats/stats.h"
#include "game/application/applicationcontext.h"

class Entity;
class ApplicationContext;

class Weapon {
protected:
    UUID id;
    std::string name;
    Entity* owner;
    DamageSource damageSource;
    Stats::WeaponStats stats;

    int usesLeft;

    ApplicationContext* context;
    EventPublisher<MeleeWeaponEventData>& publisher;

    Item* item;

    virtual bool onUse(const glm::ivec2& position, const glm::ivec2& target, bool isAnimationOnly) = 0;
    virtual void apply(const glm::ivec2& position, const glm::ivec2& target) = 0;

public:
    Weapon(
        Entity* owner,
        ApplicationContext* context,
        Item* item,
        EventPublisher<MeleeWeaponEventData>& publisher,
        const UUID& id,
        const std::string& name,
        const DamageSource& damageSource,
        const Stats::WeaponStats& stats
    );

    Weapon(
        Entity* owner, 
        ApplicationContext* context,
        Item* item,
        EventPublisher<MeleeWeaponEventData>& publisher,
        const std::string& name,
        const DamageSource& damageSource,
        const Stats::WeaponStats& stats
    );

    void use(const glm::ivec2& position, const glm::ivec2& target, bool isAnimationOnly = false);
    void reset(void);
    void setFinished(void);
    virtual bool isInRange(const glm::ivec2& position);

    virtual void update(int64_t timeSinceLastFrame) = 0;
    virtual bool hasFinished(void);
    virtual bool isAnimationInProgress(void);
    virtual Stats::WeaponStats::WeaponClass getType(void) const = 0;

    Stats::WeaponStats getStats(void) const;
    int getUsesLeft(void) const;
    void setUsesLeft(int usesLeft);

    UUID getId(void) const;
    std::string getName(void) const;
    Entity* getOwner(void);
    DamageSource getDamageSource(void) const;

    Item* getItem(void);
    void addTo(Stats::EntityStats& entityStats);
};