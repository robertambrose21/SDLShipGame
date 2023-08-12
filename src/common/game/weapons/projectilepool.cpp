#include "projectilepool.h"

ProjectilePool::ProjectilePool(AreaOfEffectPool& areaOfEffectPool) :
    areaOfEffectPool(areaOfEffectPool)
{
    loadProjectileDefinitions();
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
        definition.multiplier = data["multiplier"].get<float>();
        definition.speed = data["speed"].get<float>();
        if(data.contains("effects")) {
            auto const& effects = data["effects"].get<std::vector<json>>();

            for(auto const& effect : effects) {
                if(effect.contains("freeze")) {
                    auto duration = effect["freeze"].get<int>();

                    definition.effects.push_back(Effect("freeze", duration));
                }
            }
        }

        std::cout << "Loaded projectile definition \"" << definition.name << "\"" << std::endl;

        projectileDefinitions[definition.name] = definition;
    }

    game_assert(!projectileDefinitions.empty());
}

void ProjectilePool::add(std::unique_ptr<Projectile> projectile, Entity* owner) {
    game_assert(projectile != nullptr);
    game_assert(owner != nullptr);
    projectiles[owner].push_back(std::move(projectile));
}

Projectile::Blueprint ProjectilePool::create(const std::string& name) {
    game_assert(projectileDefinitions.contains(name));
    auto const& definition = projectileDefinitions[name];
    auto const& aoe = definition.aoe;
    Projectile::Blueprint blueprint(
        Projectile::Stats { definition.multiplier, definition.speed, definition.effects },
        name,
        definition.textureId,
        [&, aoe](auto grid, auto ownerId, auto target, auto turnNumber) { // TODO: Don't add this if there's no aoe
            if(aoe != "") {
                areaOfEffectPool.add(aoe, ownerId, turnNumber, target);
            }
        }
    );

    return blueprint;
}

void ProjectilePool::draw(GraphicsContext& graphicsContext) {
    for (auto& [owner, projectilesForOwner] : projectiles) {
        for(auto const& projectile : projectilesForOwner) {
            projectile->draw(graphicsContext);
        }
    }
}

void ProjectilePool::update(uint32_t timeSinceLastFrame) {
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

std::vector<Projectile*> ProjectilePool::getProjectilesForOwner(Entity* owner) {
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

int ProjectilePool::getNumProjectilesForOwner(Entity* owner) {
    auto projectilesForOwner = projectiles.find(owner);

    if(projectilesForOwner == projectiles.end()) {
        return 0;
    }

    return projectilesForOwner->second.size();
}