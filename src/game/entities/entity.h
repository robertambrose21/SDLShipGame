#pragma once

#include <string>

#include "core/glmimport.h"
#include "core/util/idgenerator.h"
#include "graphics/gridrenderer.h"
#include "game/weapons/weapon.h"
#include "behaviour/behaviourstrategy.h"

class Weapon;
class BehaviourStrategy;

class Entity {
public:
    typedef struct _stats {
        int movesPerTurn;
        int hp;
    } Stats;

private:
    uint32_t id;

    uint8_t textureId;
    uint8_t selectedTextureId;

    bool selected;

    std::shared_ptr<Grid> grid;

    std::shared_ptr<BehaviourStrategy> behaviourStrategy;

    glm::ivec2 position;
    std::deque<glm::ivec2> path;
    uint32_t timeSinceLastMoved;

    Stats stats;

    std::map<uint32_t, std::shared_ptr<Weapon>> weapons;
    std::shared_ptr<Weapon> currentWeapon;

    std::string name;
    int movesLeft;
    int currentHP;

    int participantId;

public:
    const uint32_t MOVES_PER_SECOND = 5;

    Entity(
        const uint32_t& id,
        const std::string& name,
        Stats stats
    );

    Entity(
        const std::string& name,
        Stats stats
    );

    static std::shared_ptr<Entity> filterByTile(
        const int& x, 
        const int& y, 
        std::set<std::shared_ptr<Entity>> entities
    ) {
        for(auto entity : entities) {
            if(entity->isOnTile(x, y)) {
                return entity;
            }
        }

        return nullptr;
    }

    static std::shared_ptr<Entity> filterByTile(
        const int& x, 
        const int& y, 
        std::map<uint32_t, std::shared_ptr<Entity>> entities
    ) {
        for(auto [entityId, entity] : entities) {
            if(entity->isOnTile(x, y)) {
                return entity;
            }
        }

        return nullptr;
    }

    void update(const uint32_t& timeSinceLastFrame, bool& quit);

    void setTextureId(const uint8_t& textureId);
    void setSelectedTextureId(const uint8_t& selectedTexture);
    uint8_t getTextureId(void) const;

    void draw(std::shared_ptr<GraphicsContext> graphicsContext);

    void setSelected(bool selected);
    bool isSelected(void) const;

    std::shared_ptr<BehaviourStrategy> getBehaviourStrategy(void);
    void setBehaviourStrategy(std::shared_ptr<BehaviourStrategy> behaviourStrategy);

    Stats getStats(void) const;
    const float getSpeed(void);
    int getCurrentHP(void) const;
    void setCurrentHP(const int& hp);
    void takeDamage(const int& amount);
    void attack(std::shared_ptr<Entity> target, std::shared_ptr<Weapon> weapon);

    std::map<uint32_t, std::shared_ptr<Weapon>> getWeapons(void) const;
    std::shared_ptr<Weapon> getWeapon(const uint32_t& weaponId);
    std::shared_ptr<Weapon> addWeapon(std::shared_ptr<Weapon> weapon);
    void removeWeapon(const std::string& name);
    void setCurrentWeapon(std::shared_ptr<Weapon> weapon);
    std::shared_ptr<Weapon> getCurrentWeapon(void);

    uint32_t getId(void) const;
    void setId(const uint32_t& id);
    std::string getName(void) const;
    void setName(const std::string& name);

    glm::ivec2 getPosition(void) const;
    bool isOnTile(const int& x, const int& y);

    void setPosition(const glm::ivec2& position);
    bool findPath(const glm::ivec2& target, const int& stopShortSteps = 0);
    bool isNeighbour(std::shared_ptr<Entity> entity) const;

    int getMovesLeft(void) const;
    void setMovesLeft(const int& movesLeft);
    bool isTurnInProgress(void) const;
    void useMoves(const int& numMoves);

    void setParticipantId(int participantId);
    int getParticipantId(void) const;

    void reset(void);
    void nextTurn(void);
    bool endTurnCondition(void);
};
