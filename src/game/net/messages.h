#pragma once

#include <cstdint>
#include <cstring>
#include "game/entities/entity.h"
#include "game/entities/enemy.h"
#include "player/player.h"

const int MaxEntities = 64;
const int MaxEntityNameLength = 64;

class Entity;
class Enemy;
class Player;

struct EntityStateUpdate {
    uint32_t id;
    uint8_t textureId;
    char name[MaxEntityNameLength];
    int totalHP;
    int currentHP;
    int x, y;
    int movesPerTurn;
    int movesLeft;
    // TODO: Weapons

    EntityStateUpdate() {
        memset(this, 0, sizeof(EntityStateUpdate));
    }

    static EntityStateUpdate serialize(std::shared_ptr<Entity> entity) {
        EntityStateUpdate entityStateUpdate;

        entityStateUpdate.id = entity->getId();
        strcpy(entityStateUpdate.name, entity->getName().c_str());
        entityStateUpdate.textureId = entity->getTextureId();
        entityStateUpdate.totalHP = entity->getStats().hp;
        entityStateUpdate.currentHP = entity->getCurrentHP();
        entityStateUpdate.x = entity->getPosition().x;
        entityStateUpdate.y = entity->getPosition().y;
        entityStateUpdate.movesPerTurn = entity->getStats().movesPerTurn;
        entityStateUpdate.movesLeft = entity->getMovesLeft();

        return entityStateUpdate;
    }

    // TODO: Entity constructor which sets the id
    static std::shared_ptr<Entity> deserialize(EntityStateUpdate update, std::shared_ptr<Entity> existing) {
        if(existing == nullptr) {
            // TODO: Remove enemy/player classes and just have entity have "tags" which
            // define strategies e.g. following the closest target
            if(update.name == "Space Worm") {
                // What the fuck - I guess we're removing derived entity classes anyway. So long as it compiles
                existing = std::dynamic_pointer_cast<Entity, Enemy>(std::make_shared<Enemy>(
                    update.name, 
                    Entity::Stats { update.movesPerTurn, update.totalHP }
                ));
            }
            else {
                existing = std::make_shared<Player>();
            }

            existing->setName(update.name);
            existing->setId(update.id);
        }

        existing->setPosition(glm::ivec2(update.x, update.y));
        existing->setTextureId(update.textureId);
        existing->setCurrentHP(update.currentHP);
        existing->setMovesLeft(update.movesLeft);

        return existing;
    }
};

struct GameStateUpdate {
    int numEntities;
    EntityStateUpdate entities[MaxEntities];

    GameStateUpdate() {
        memset(this, 0, sizeof(GameStateUpdate));
    }

    static GameStateUpdate serialize(std::map<uint32_t, std::shared_ptr<Entity>> entities) {
        GameStateUpdate update;
        update.numEntities = entities.size();

        int index = 0;
        for(auto [entityId, entity] : entities) {
            update.entities[index++] = EntityStateUpdate::serialize(entity);
        }

        return update;
    }
};

const int MaxMapBlockSize = 512;

struct MapBlock {
    int width;
    int height;
    int totalSize;
    int sequence;
    int blockSize;

    int data[MaxMapBlockSize];

    MapBlock() {
        memset(this, 0, sizeof(MapBlock));
    }
};