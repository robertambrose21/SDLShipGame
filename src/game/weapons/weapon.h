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
    std::shared_ptr<Entity> owner;
    Stats stats;

    int usesLeft;

    std::shared_ptr<Grid> grid;

    virtual void onUse(glm::ivec2 position, std::shared_ptr<Entity> target) = 0;

public:
    Weapon(
        std::shared_ptr<Entity> owner,
        std::shared_ptr<Grid> grid,
        const uint32_t& id,
        const std::string& name, 
        Stats stats
    );

    Weapon(
        std::shared_ptr<Entity> owner,
        std::shared_ptr<Grid> grid, 
        const std::string& name, 
        Stats stats
    );

    void use(glm::ivec2 position, std::shared_ptr<Entity> target);
    void reset(void);
    void setFinished(void);
    bool isInRange(glm::ivec2 position);

    virtual void draw(std::shared_ptr<GraphicsContext> graphicsContext) = 0;
    virtual void update(const uint32_t& timeSinceLastFrame) = 0;
    virtual bool hasFinished(void);
    virtual Type getType(void) const = 0;

    Stats getStats(void) const;
    int getUsesLeft(void) const;
    void setUsesLeft(const int& usesLeft);

    uint32_t getId(void) const;
    std::string getName(void) const;
    std::shared_ptr<Entity> getOwner(void);
};