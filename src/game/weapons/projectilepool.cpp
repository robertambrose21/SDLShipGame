#include "projectilepool.h"

ProjectilePool::ProjectilePool(std::shared_ptr<AreaOfEffectPool> areaOfEffectPool) :
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
        definition.textureId = data["textureId"].get<uint8_t>();
        definition.multiplier = data["multiplier"].get<float>();
        definition.speed = data["speed"].get<float>();

        std::cout << "Loaded projectile definition \"" << definition.name << "\"" << std::endl;

        projectileDefinitions[definition.name] = definition;
    }
}

void ProjectilePool::add(std::shared_ptr<Projectile> projectile, std::shared_ptr<Entity> owner) {
    projectiles[owner].push_back(projectile);
}

Projectile::Blueprint ProjectilePool::create(const std::string& name) {
    if(!projectileDefinitions.contains(name)) {
        throw std::runtime_error("Could not find projectile definition with name " + name);
    }

    auto definition = projectileDefinitions[name];
    auto aoe = definition.aoe;
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

void ProjectilePool::draw(std::shared_ptr<GraphicsContext> graphicsContext) {
    for(auto [owner, projectilesForOwner] : projectiles) {
        for(auto projectile : projectilesForOwner) {
            projectile->draw(graphicsContext);
        }
    }
}

void ProjectilePool::update(const uint32_t& timeSinceLastFrame) {
    for(auto [owner, projectilesForOwner] : projectiles) {
        for(auto projectile : projectilesForOwner) {
            projectile->update(timeSinceLastFrame);

            if(projectile->hasReachedTarget()) {
                projectilesForDeletion[owner].push_back(projectile);
            }
        }
    }

    for(auto [owner, projectilesForOwner] : projectilesForDeletion) {
        for(auto projectile : projectilesForOwner) {
            projectiles[owner].erase(std::find(projectiles[owner].begin(), projectiles[owner].end(), projectile));
        }
    }
    
    projectilesForDeletion.clear();
}

std::vector<std::shared_ptr<Projectile>> ProjectilePool::getProjectilesForOwner(std::shared_ptr<Entity> owner) {
    return projectiles[owner];
}