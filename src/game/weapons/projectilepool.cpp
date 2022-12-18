#include "projectilepool.h"

ProjectilePool::ProjectilePool(const std::shared_ptr<AreaOfEffectPool>& areaOfEffectPool) :
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

        std::cout << "Loaded projectile definition \"" << definition.name << "\"" << std::endl;

        projectileDefinitions[definition.name] = definition;
    }

    game_assert(!projectileDefinitions.empty());
}

void ProjectilePool::add(const std::shared_ptr<Projectile>& projectile, const std::shared_ptr<Entity>& owner) {
    game_assert(projectile != nullptr);
    game_assert(owner != nullptr);
    projectiles[owner].push_back(projectile);
}

Projectile::Blueprint ProjectilePool::create(const std::string& name) {
    game_assert(projectileDefinitions.contains(name));
    auto const& definition = projectileDefinitions[name];
    auto const& aoe = definition.aoe;
    Projectile::Blueprint blueprint(
        Projectile::Stats { definition.multiplier, definition.speed },
        name,
        definition.textureId,
        [&, aoe](auto grid, auto entity, auto turnNumber) { // TODO: Don't add this if there's no aoe
            areaOfEffectPool->add(aoe, turnNumber, entity->getPosition());
        }
    );

    return blueprint;
}

void ProjectilePool::draw(const std::shared_ptr<GraphicsContext>& graphicsContext) {
    game_assert(graphicsContext != nullptr);
    for (auto [owner, projectilesForOwner] : projectiles) {
        for(auto const& projectile : projectilesForOwner) {
            projectile->draw(graphicsContext);
        }
    }
}

void ProjectilePool::update(uint32_t timeSinceLastFrame) {
    for(auto [owner, projectilesForOwner] : projectiles) {
        for(auto const& projectile : projectilesForOwner) {
            projectile->update(timeSinceLastFrame);

            if(projectile->hasReachedTarget()) {
                projectilesForDeletion[owner].push_back(projectile);
            }
        }
    }

    for(auto [owner, projectilesForOwner] : projectilesForDeletion) {
        for(auto const& projectile : projectilesForOwner) {
            projectiles[owner].erase(std::find(projectiles[owner].begin(), projectiles[owner].end(), projectile));
        }
    }
    
    projectilesForDeletion.clear();
}

const std::vector<std::shared_ptr<Projectile>>& ProjectilePool::getProjectilesForOwner(const std::shared_ptr<Entity>& owner) const {
    game_assert(owner != nullptr);
    static const std::vector<std::shared_ptr<Projectile>> empty;

    auto const& projectilesForOwner = projectiles.find(owner);
    
    if(projectilesForOwner != projectiles.end()) {
        return projectilesForOwner->second;
    }

    return empty;
}