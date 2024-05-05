#pragma once

#include <string>

#include "core/glmimport.h"
#include "core/util/randomutils.h"
#include "game/application/applicationcontext.h"
#include "game/entities/entitypool.h"

class SpawnController {
public:
    typedef struct _spawnBox {
        glm::ivec2 min;
        glm::ivec2 max;
    } SpawnBox;

private:
    ApplicationContext* context;
    bool initialised;

    glm::ivec2 getRandomPositionFromSpawnBox(const SpawnBox& spawnBox);

public:
    SpawnController();

    void initialise(ApplicationContext& context);

    void spawnEntities(const std::string& name, const SpawnBox spawnBox, int count = 1);
    void spawnEntity(const std::string& name, const glm::ivec2& position);
};