#pragma once

#include <string>
#include <functional>
#include <memory>

#include "game/entities/entity.h"
#include "projectile.h"

class Entity;
class Projectile;

class Weapon {
public:
    enum Type {
        MELEE,
        PROJECTILE
    };

    typedef struct _stats {
        int damage;
        int range;
        int uses;
    } Stats;

protected:
    uint32_t id;
    std::string name;
    Entity* owner;
    Stats stats;

    int usesLeft;

    Grid& grid;

    virtual bool onUse(const glm::ivec2& position, const glm::ivec2& target) = 0;

public:
    Weapon(
        Entity* owner,
        Grid& grid,
        uint32_t id,
        const std::string& name, 
        const Stats& stats
    );

    Weapon(
        Entity* owner,
        Grid& grid, 
        const std::string& name, 
        const Stats& stats
    );

    void use(const glm::ivec2& position, const glm::ivec2& target);
    void reset(void);
    void setFinished(void);
    bool isInRange(const glm::ivec2& position);

    virtual void draw(GraphicsContext& graphicsContext) = 0;
    virtual void update(uint32_t timeSinceLastFrame) = 0;
    virtual bool hasFinished(void);
    virtual Type getType(void) const = 0;

    Stats getStats(void) const;
    int getUsesLeft(void) const;
    void setUsesLeft(int usesLeft);

    uint32_t getId(void) const;
    std::string getName(void) const;
    Entity* getOwner(void);
};