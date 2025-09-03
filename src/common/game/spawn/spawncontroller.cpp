#include "spawncontroller.h"

SpawnController::SpawnController() :
    initialised(false)
{ }

void SpawnController::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

std::vector<entt::entity> SpawnController::spawnActors(const std::string& name, const SpawnBox& spawnBox, int count) {
    game_assert(count >= 1);

    std::vector<entt::entity> actors;

    for(auto i = 0; i < count; i++) {
        auto entity = context->getActorPool()->addActor(name);
        context->getEntityRegistry().replace<Position>(entity, getRandomPositionFromSpawnBox(spawnBox));
        context->getEntityRegistry().get_or_emplace<PositionDirty>(entity);
        actors.push_back(entity);
    }

    return actors;
}

std::vector<entt::entity> SpawnController::spawnActors(const SpawnableActors& spawnables, const SpawnBox& spawnBox, int count) {
    game_assert(count >= 1);

    std::vector<entt::entity> actors;

    for(auto i = 0; i < count; i++) {
        SpawnableActor spawnable;
        if(spawnables.weights.empty()) {
            spawnable = randomChoice(spawnables.actors);
        }
        else {
            spawnable = randomChoice(spawnables.actors, spawnables.weights);
        }

        auto entity = spawnActor(spawnable.name, getRandomPositionFromSpawnBox(spawnBox));
        auto& actor = context->getEntityRegistry().get<Actor>(entity);

        for(auto weaponName : spawnable.weapons) {
            auto weapon = context->getWeaponController()->createWeapon(weaponName, entity);
            actor.addWeapon(std::move(weapon));
        }

        actors.push_back(entity);
    }

    return actors;
}

entt::entity SpawnController::spawnActor(const std::string& name, const glm::ivec2& position) {
    auto entity = context->getActorPool()->addActor(name);
    context->getEntityRegistry().replace<Position>(entity, position);
    context->getEntityRegistry().get_or_emplace<PositionDirty>(entity);
    return entity;
}

glm::ivec2 SpawnController::getRandomPositionFromSpawnBox(const SpawnBox& spawnBox) {
    return glm::vec2(randomRange(spawnBox.min.x, spawnBox.max.x), randomRange(spawnBox.min.y, spawnBox.max.y));
}