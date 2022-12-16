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
    ProjectilePool(const std::shared_ptr<AreaOfEffectPool>& areaOfEffectPool);

    void add(const std::shared_ptr<Projectile>& projectile, const std::shared_ptr<Entity>& owner);
    Projectile::Blueprint create(const std::string& name);

    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);
    void update(const uint32_t& timeSinceLastFrame);

    const std::vector<std::shared_ptr<Projectile>>& getProjectilesForOwner(const std::shared_ptr<Entity>& owner) const;
};