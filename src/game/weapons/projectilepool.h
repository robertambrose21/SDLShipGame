#pragma once

#include <map>
#include <vector>

#include "core/json.hpp"
#include "projectile.h"
#include "game/entities/entity.h"
#include "areaofeffectpool.h"
#include "game/effects/effect.h"

using json = nlohmann::json;

class ProjectilePool {
private:
    typedef struct _projectileDefinition {
        std::string filename;
        std::string name;
        std::string aoe;
        uint32_t textureId;
        float multiplier;
        float speed;
        std::vector<Effect> effects;
    } ProjectileDefinition;

    std::map<std::string, ProjectileDefinition> projectileDefinitions;

    std::map<Entity*, std::vector<std::unique_ptr<Projectile>>> projectiles;
    std::map<Entity*, std::vector<int>> projectilesForDeletion;

    AreaOfEffectPool& areaOfEffectPool;

    void loadProjectileDefinitions(void);

public:
    ProjectilePool(AreaOfEffectPool& areaOfEffectPool);

    void add(std::unique_ptr<Projectile> projectile, Entity* owner);
    Projectile::Blueprint create(const std::string& name);

    void draw(GraphicsContext& graphicsContext);
    void update(uint32_t timeSinceLastFrame);

    std::vector<Projectile*> getProjectilesForOwner(Entity* owner);
    int getNumProjectilesForOwner(Entity* owner);
};