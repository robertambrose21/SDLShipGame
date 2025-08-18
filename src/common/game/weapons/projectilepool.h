#pragma once

#include <map>
#include <vector>

#include "core/json.hpp"
#include "projectile.h"
#include "game/actors/actor.h"
#include "areaofeffectpool.h"
#include "game/effects/effect.h"
#include "core/event/eventpublisher.h"
#include "game/application/applicationcontext.h"

using json = nlohmann::json;

class ProjectilePool : public EventPublisher<ProjectileEventData> {
private:
    typedef struct _effectDefinition {
        EffectType type;
        uint8_t duration;
        std::vector<uint32_t> damageTicks;
    } EffectDefintion;

    typedef struct _projectileDefinition {
        std::string filename;
        std::string name;
        std::string aoe;
        uint32_t textureId;
        float multiplier;
        float speed;
        std::vector<EffectDefintion> effects;
    } ProjectileDefinition;

    std::map<std::string, ProjectileDefinition> projectileDefinitions;

    std::map<Actor*, std::vector<std::unique_ptr<Projectile>>> projectiles;
    std::map<Actor*, std::vector<int>> projectilesForDeletion;

    ApplicationContext* context;
    bool initialised;

    void loadProjectileDefinitions(void);
    std::function<void(int, const glm::ivec2&, bool)> buildOnHitCallback(const ProjectileDefinition& definition);
    std::vector<Stats::EffectStats> buildEffectStats(const ProjectileDefinition& definition);

public:
    ProjectilePool();

    void initialise(ApplicationContext& context);
    void add(std::unique_ptr<Projectile> projectile, Actor* owner);
    Projectile::Blueprint create(const std::string& name);

    void update(int64_t timeSinceLastFrame);

    std::vector<Projectile*> getProjectilesForOwner(Actor* owner);
    std::vector<Projectile*> getAllProjectiles(void);
    int getNumProjectilesForOwner(Actor* owner);
};