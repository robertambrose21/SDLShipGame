#pragma once

#include <map>
#include <vector>

#include "core/json.hpp"
#include "projectile.h"
#include "game/entities/entity.h"
#include "areaofeffectpool.h"

using json = nlohmann::json;

class ProjectilePool {
private:
    typedef struct _projectileDefinition {
        std::string filename;
        std::string name;
        std::string aoe;
        uint8_t textureId;
        float multiplier;
        float speed;
    } ProjectileDefinition;

    std::map<std::string, ProjectileDefinition> projectileDefinitions;

    std::map<std::shared_ptr<Entity>, std::vector<std::shared_ptr<Projectile>>> projectiles;
    std::map<std::shared_ptr<Entity>, std::vector<std::shared_ptr<Projectile>>> projectilesForDeletion;

    std::shared_ptr<AreaOfEffectPool> areaOfEffectPool;

    void loadProjectileDefinitions(void);

public:
    ProjectilePool(std::shared_ptr<AreaOfEffectPool> areaOfEffectPool);

    void add(std::shared_ptr<Projectile> projectile, std::shared_ptr<Entity> owner);
    Projectile::Blueprint create(const std::string& name);

    void draw(std::shared_ptr<GraphicsContext> graphicsContext);
    void update(const uint32_t& timeSinceLastFrame);

    std::vector<std::shared_ptr<Projectile>> getProjectilesForOwner(std::shared_ptr<Entity> owner);
};