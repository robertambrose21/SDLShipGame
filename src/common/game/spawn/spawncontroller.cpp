#include "spawncontroller.h"

SpawnController::SpawnController() :
    initialised(false)
{ }

void SpawnController::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

void SpawnController::spawnEntities(const std::string& name, const SpawnBox spawnBox, int count) {
    game_assert(count >= 1);

    for(auto i = 0; i < count; i++) {
        auto entity = context->getEntityPool()->addEntity(name);
        entity->setPosition(getRandomPositionFromSpawnBox(spawnBox));
    }
}

void SpawnController::spawnEntity(const std::string& name, const glm::ivec2& position) {
    auto entity = context->getEntityPool()->addEntity(name);
    entity->setPosition(position);
}

glm::ivec2 SpawnController::getRandomPositionFromSpawnBox(const SpawnBox& spawnBox) {
    return glm::vec2(randomRange(spawnBox.min.x, spawnBox.max.x), randomRange(spawnBox.min.y, spawnBox.max.y));
}