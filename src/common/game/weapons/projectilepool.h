#pragma once

#include <map>
#include <vector>

#include "core/json.hpp"
#include "projectile.h"
#include "game/entities/entity.h"
#include "areaofeffectpool.h"
#include "game/effects/effect.h"
#include "core/event/eventpublisher.h"
#include "game/application/applicationcontext.h"

using json = nlohmann::json;

class ProjectilePool : public EventPublisher<ProjectileEventData> {
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

    ApplicationContext* context;
    bool initialised;

    void loadProjectileDefinitions(void);

public:
    ProjectilePool();

    void initialise(ApplicationContext& context);
    void add(std::unique_ptr<Projectile> projectile, Entity* owner);
    Projectile::Blueprint create(const std::string& name);

    void update(int64_t timeSinceLastFrame);

    std::vector<Projectile*> getProjectilesForOwner(Entity* owner);
    std::vector<Projectile*> getAllProjectiles(void);
    int getNumProjectilesForOwner(Entity* owner);
};