#include "spawncontroller.h"

SpawnController::SpawnController() :
    initialised(false)
{ }

void SpawnController::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

std::vector<Actor*> SpawnController::spawnActors(const std::string& name, const SpawnBox& spawnBox, int count) {
    game_assert(count >= 1);

    std::vector<Actor*> actors;

    for(auto i = 0; i < count; i++) {
        auto actor = context->getActorPool()->addActor(name);
        actor->setPosition(getRandomPositionFromSpawnBox(spawnBox));
        actors.push_back(actor);
    }

    return actors;
}

std::vector<Actor*> SpawnController::spawnActors(const SpawnableActors& spawnables, const SpawnBox& spawnBox, int count) {
    game_assert(count >= 1);

    std::vector<Actor*> actors;

    for(auto i = 0; i < count; i++) {
        SpawnableActor spawnable;
        if(spawnables.weights.empty()) {
            spawnable = randomChoice(spawnables.actors);
        }
        else {
            spawnable = randomChoice(spawnables.actors, spawnables.weights);
        }

        auto actor = spawnActor(spawnable.name, getRandomPositionFromSpawnBox(spawnBox));

        for(auto weaponName : spawnable.weapons) {
            auto weapon = context->getWeaponController()->createWeapon(weaponName, actor);
            actor->addWeapon(std::move(weapon));
        }

        actors.push_back(actor);
    }

    return actors;
}

Actor* SpawnController::spawnActor(const std::string& name, const glm::ivec2& position) {
    auto actor = context->getActorPool()->addActor(name);
    actor->setPosition(position);
    return actor;
}

glm::ivec2 SpawnController::getRandomPositionFromSpawnBox(const SpawnBox& spawnBox) {
    return glm::vec2(randomRange(spawnBox.min.x, spawnBox.max.x), randomRange(spawnBox.min.y, spawnBox.max.y));
}