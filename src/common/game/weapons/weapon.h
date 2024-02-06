#pragma once

#include <string>
#include <functional>
#include <memory>

#include "damagesource.h"
#include "game/entities/entity.h"
#include "core/event/eventpublisher.h"
#include "core/grid/grid.h"
#include "game/event/events.h"
#include "game/items/equipment.h"

class Entity;
class EntityPool;

class Weapon {
public:
    // TODO: Probably a better way to do abstraction than this
    enum Type {
        MELEE,
        PROJECTILE
    };

    typedef struct _stats {
        int range;
        int uses;
    } Stats;

protected:
    UUID id;
    std::string name;
    Entity* owner;
    Stats stats;
    DamageSource damageSource;

    int usesLeft;

    Grid* grid;
    EntityPool* entityPool;
    EventPublisher<WeaponEventData>& publisher;

    Item* item;

    virtual bool onUse(const glm::ivec2& position, const glm::ivec2& target) = 0;

public:
    Weapon(
        Entity* owner,
        Grid* grid,
        EntityPool* entityPool,
        Item* item,
        EventPublisher<WeaponEventData>& publisher,
        const UUID& id,
        const std::string& name, 
        const Stats& stats,
        const DamageSource& damageSource
    );

    Weapon(
        Entity* owner, 
        Grid* grid,
        EntityPool* entityPool,
        Item* item,
        EventPublisher<WeaponEventData>& publisher,
        const std::string& name, 
        const Stats& stats,
        const DamageSource& damageSource
    );

    void use(const glm::ivec2& position, const glm::ivec2& target);
    void reset(void);
    void setFinished(void);
    virtual bool isInRange(const glm::ivec2& position);

    virtual void update(int64_t timeSinceLastFrame) = 0;
    virtual bool hasFinished(void);
    virtual bool isAnimationInProgress(void);
    virtual Type getType(void) const = 0;

    Stats getStats(void) const;
    int getUsesLeft(void) const;
    void setUsesLeft(int usesLeft);

    UUID getId(void) const;
    std::string getName(void) const;
    Entity* getOwner(void);
    DamageSource getDamageSource(void) const;

    Item* getItem(void);
};