#pragma once

#include <string>
#include <vector>

#include "core/glmimport.h"
#include "core/util/randomutils.h"
#include "game/application/applicationcontext.h"
#include "game/actors/actorpool.h"
#include "game/weapons/weaponcontroller.h"

class SpawnController {
public:
    typedef struct _spawnBox {
        glm::ivec2 min;
        glm::ivec2 max;
    } SpawnBox;

    typedef struct _spawnableActor {
        std::string name;
        std::vector<std::string> weapons;
    } SpawnableActor;

    typedef struct _spawnableActors {
        std::vector<SpawnableActor> actors;
        std::vector<int> weights;
    } SpawnableActors;

private:
    ApplicationContext* context;
    bool initialised;

    glm::ivec2 getRandomPositionFromSpawnBox(const SpawnBox& spawnBox);

public:
    SpawnController();

    void initialise(ApplicationContext& context);

    std::vector<Actor*> spawnActors(const std::string& name, const SpawnBox& spawnBox, int count = 1);
    std::vector<Actor*> spawnActors(const SpawnableActors& spawnables, const SpawnBox& spawnBox, int count = 1);
    Actor* spawnActor(const std::string& name, const glm::ivec2& position);
};