#include "spawncontroller.h"

SpawnController::SpawnController() :
    initialised(false)
{ }

void SpawnController::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

std::vector<Entity*> SpawnController::spawnEntities(const std::string& name, const SpawnBox& spawnBox, int count) {
    game_assert(count >= 1);

    std::vector<Entity*> entities;

    for(auto i = 0; i < count; i++) {
        auto entity = context->getEntityPool()->addEntity(name);
        entity->setPosition(getRandomPositionFromSpawnBox(spawnBox));
        entities.push_back(entity);
    }

    return entities;
}

std::vector<Entity*> SpawnController::spawnEntities(const SpawnableEntities& spawnables, const SpawnBox& spawnBox, int count) {
    game_assert(count >= 1);

    std::vector<Entity*> entities;

    for(auto i = 0; i < count; i++) {
        SpawnableEntity spawnable;
        if(spawnables.weights.empty()) {
            spawnable = randomChoice(spawnables.entities);
        }
        else {
            spawnable = randomChoice(spawnables.entities, spawnables.weights);
        }

        auto entity = spawnEntity(spawnable.name, getRandomPositionFromSpawnBox(spawnBox));

        for(auto weaponName : spawnable.weapons) {
            auto weapon = context->getWeaponController()->createWeapon(weaponName, entity);
            entity->addWeapon(std::move(weapon));
        }

        entities.push_back(entity);
    }

    return entities;
}

Entity* SpawnController::spawnEntity(const std::string& name, const glm::ivec2& position) {
    auto entity = context->getEntityPool()->addEntity(name);
    entity->setPosition(position);
    return entity;
}

glm::ivec2 SpawnController::getRandomPositionFromSpawnBox(const SpawnBox& spawnBox) {
    return glm::vec2(randomRange(spawnBox.min.x, spawnBox.max.x), randomRange(spawnBox.min.y, spawnBox.max.y));
}