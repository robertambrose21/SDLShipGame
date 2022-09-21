#include "weapon.h"

Weapon::Weapon(std::shared_ptr<GridRenderer> grid, const std::string& name, Stats stats) :
    grid(grid),
    name(name),
    stats(stats)
{ }

// TODO: Melee and projectile weapons
void Weapon::use(glm::ivec2 position, std::shared_ptr<Entity> target) {
    if(projectileTexture != nullptr) {
        projectiles.push_back(std::make_shared<Projectile>(
            grid, 
            projectileTexture, 
            position, 
            target,
            Projectile::Stats { 1, 2 },
            [&](auto target) {
                target->takeDamage(stats.damage); 
            }
        ));
    }
}

void Weapon::draw(std::shared_ptr<SDL_Renderer> renderer) {
    for(auto projectile : projectiles) {
        projectile->draw(renderer);
    }
}

void Weapon::update(const Uint32& timeSinceLastFrame) {
    for(auto projectile : projectiles) {
        projectile->update(timeSinceLastFrame);

        if(projectile->hasReachedTarget()) {
            projectilesForDeletion.push_back(projectile);
        }
    }

    for(auto projectile : projectilesForDeletion) {
        projectiles.erase(std::find(projectiles.begin(), projectiles.end(), projectile));
    }
    
    projectilesForDeletion.clear();
}

void Weapon::setProjectileTexture(std::shared_ptr<Texture> projectileTexture) {
    this->projectileTexture = projectileTexture;
}

Weapon::Stats Weapon::getStats(void) const {
    return stats;
}

std::string Weapon::getName(void) const {
    return name;
}