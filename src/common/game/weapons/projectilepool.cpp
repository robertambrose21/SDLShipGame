#include "projectilepool.h"

ProjectilePool::ProjectilePool() {
    loadProjectileDefinitions();
}

void ProjectilePool::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

void ProjectilePool::loadProjectileDefinitions(void) {
    std::string directory = "../assets/data/projectiles";

    for(const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::ifstream f(entry.path());
        json data = json::parse(f);

        ProjectileDefinition definition;
        definition.filename = entry.path();
        definition.name = data["name"].get<std::string>();
        if(data.contains("aoe")) {
            definition.aoe = data["aoe"].get<std::string>();
        }
        else {
            definition.aoe = "";
        }
        definition.textureId = data["textureId"].get<uint32_t>();
        definition.speed = data["speed"].get<float>();
        if(data.contains("effects")) {
            auto effects = data["effects"].get<std::vector<json>>();

            for(auto const& effect : effects) {
                if(effect.contains("freeze")) {
                    auto freeze = effect["freeze"].get<json>();
                    auto duration = freeze["duration"].get<uint8_t>();
                    definition.effects.push_back({ EffectType::FREEZE, duration });
                }
                if(effect.contains("poison")) {
                    auto poison = effect["poison"].get<json>();
                    auto duration = poison["duration"].get<uint8_t>();
                    auto damagePerTick = poison["damagePerTick"].get<uint32_t>();
                    
                    std::vector<uint32_t> damageTicks(duration);
                    std::fill(damageTicks.begin(), damageTicks.end(), damagePerTick);
                    definition.effects.push_back({ EffectType::POISON, duration, damageTicks });
                }
            }
        }

        std::cout << "Loaded projectile definition \"" << definition.name << "\"" << std::endl;

        projectileDefinitions[definition.name] = definition;
    }

    game_assert(!projectileDefinitions.empty());
}

std::function<void(int, const glm::ivec2&, bool)> ProjectilePool::buildOnHitCallback(const ProjectileDefinition& definition) {
    auto const& aoe = definition.aoe;
    
    return [&](auto ownerId, auto target, auto isAnimationOnly) {
        if(aoe != "") {
            context->getAreaOfEffectPool()->add(aoe, ownerId, target, isAnimationOnly);
        }
    };
}

// TODO: Should be handled by effect controller
std::vector<Stats::EffectStats> ProjectilePool::buildEffectStats(const ProjectileDefinition& definition) {
    std::vector<Stats::EffectStats> effects;

    for(auto effect : definition.effects) {
        effects.push_back({ effect.type, effect.duration, effect.damageTicks });
    }

    return effects;
}

void ProjectilePool::add(std::unique_ptr<Projectile> projectile, Actor* owner) {
    game_assert(initialised);
    game_assert(projectile != nullptr);
    game_assert(owner != nullptr);
    projectiles[owner].push_back(std::move(projectile));
}

Projectile::Blueprint ProjectilePool::create(const std::string& name) {
    game_assert(initialised);
    game_assert(projectileDefinitions.contains(name));

    auto const& definition = projectileDefinitions[name];
    auto const& aoe = definition.aoe;
    
    Stats::ProjectileStats stats;
    stats.speed = definition.speed;
    stats.effects = buildEffectStats(definition);

    if(aoe != "") {
        stats.aoe = context->getAreaOfEffectPool()->getStatsFor(aoe);
    }

    Projectile::Blueprint blueprint(
        stats,
        name,
        definition.textureId,
        buildOnHitCallback(definition)
    );

    return blueprint;
}

void ProjectilePool::update(int64_t timeSinceLastFrame) {
    game_assert(initialised);

    for(auto [owner, projectilesForOwnerIds] : projectilesForDeletion) {
        for(auto const& projectileId : projectilesForOwnerIds) {
            projectiles[owner].erase(projectiles[owner].begin() + projectileId);
        }
    }
    
    projectilesForDeletion.clear();

    for(auto& [owner, projectilesForOwner] : projectiles) {
        for(int i = 0; i < projectilesForOwner.size(); i++) {
            projectilesForOwner[i]->update(timeSinceLastFrame);

            if(projectilesForOwner[i]->hasReachedTarget()) {
                projectilesForDeletion[owner].push_back(i);
            }
        }
    }
}

std::vector<Projectile*> ProjectilePool::getProjectilesForOwner(Actor* owner) {
    game_assert(initialised);
    game_assert(owner != nullptr);

    std::vector<Projectile*> vProjectiles;

    auto projectilesForOwner = projectiles.find(owner);

    if(projectilesForOwner == projectiles.end()) {
        return vProjectiles;
    }
    
    for(auto& projectile : projectilesForOwner->second) {
        vProjectiles.push_back(projectile.get());
    }

    return vProjectiles;
}

std::vector<Projectile*> ProjectilePool::getAllProjectiles(void) {
    game_assert(initialised);

    std::vector<Projectile*> vProjectiles;

    for (auto& [owner, projectilesForOwner] : projectiles) {
        for(auto const& projectile : projectilesForOwner) {
            vProjectiles.push_back(projectile.get());
        }
    }

    return vProjectiles;
}

int ProjectilePool::getNumProjectilesForOwner(Actor* owner) {
    game_assert(initialised);

    auto projectilesForOwner = projectiles.find(owner);

    if(projectilesForOwner == projectiles.end()) {
        return 0;
    }

    return projectilesForOwner->second.size();
}