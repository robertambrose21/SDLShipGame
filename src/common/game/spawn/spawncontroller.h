#pragma once

#include <string>
#include <vector>

#include "core/glmimport.h"
#include "core/util/randomutils.h"
#include "game/application/applicationcontext.h"
#include "game/entities/entitypool.h"
#include "game/weapons/weaponcontroller.h"

class SpawnController {
public:
    typedef struct _spawnBox {
        glm::ivec2 min;
        glm::ivec2 max;
    } SpawnBox;

    typedef struct _spawnableEntity {
        std::string name;
        std::vector<std::string> weapons;
    } SpawnableEntity;

    typedef struct _spawnableEntities {
        std::vector<SpawnableEntity> entities;
        std::vector<int> weights;
    } SpawnableEntities;

private:
    ApplicationContext* context;
    bool initialised;

    glm::ivec2 getRandomPositionFromSpawnBox(const SpawnBox& spawnBox);

public:
    SpawnController();

    void initialise(ApplicationContext& context);

    std::vector<Entity*> spawnEntities(const std::string& name, const SpawnBox& spawnBox, int count = 1);
    std::vector<Entity*> spawnEntities(const SpawnableEntities& spawnables, const SpawnBox& spawnBox, int count = 1);
    Entity* spawnEntity(const std::string& name, const glm::ivec2& position);
};