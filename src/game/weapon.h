#pragma once

#include <string>
#include <functional>
#include <memory>

#include "entity.h"
#include "projectile.h"

class Entity;
class Projectile;

class Weapon {
public:
    typedef struct _stats {
        int damage;
        int range;
        int rounds;
    } Stats;

private:
    std::string name;
    Stats stats;

    std::shared_ptr<Texture> projectileTexture;
    std::vector<std::shared_ptr<Projectile>> projectiles;
    std::vector<std::shared_ptr<Projectile>> projectilesForDeletion;

    std::shared_ptr<GridRenderer> grid;

public:
    Weapon(std::shared_ptr<GridRenderer> grid, const std::string& name, Stats stats);

    void use(glm::ivec2 position, std::shared_ptr<Entity> target);
    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);

    void setProjectileTexture(std::shared_ptr<Texture> projectileTexture);

    Stats getStats(void) const;
    std::string getName(void) const;
};