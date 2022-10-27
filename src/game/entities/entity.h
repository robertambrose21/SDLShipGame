#pragma once

#include <string>

#include "core/glmimport.h"
#include "graphics/gridrenderer.h"
#include "game/weapons/weapon.h"

class Weapon;

class Entity {
public:
    typedef struct _stats {
        int movesPerTurn;
        int hp;
    } Stats;

private:
    uint8_t textureId;
    uint8_t selectedTextureId;

    bool selected;

    glm::ivec2 position;
    std::deque<glm::ivec2> path;
    Uint32 timeSinceLastMoved;

    Stats stats;
    std::vector<std::shared_ptr<Weapon>> weapons;

    std::shared_ptr<Weapon> currentWeapon;

    std::string name;
    int movesLeft;
    int currentHP;

    int participantId;

protected:
    std::shared_ptr<Grid> grid;

    virtual void additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) = 0;

public:
    const Uint32 MOVES_PER_SECOND = 5;

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

    void update(const Uint32& timeSinceLastFrame, bool& quit);

    void setTextureId(const uint8_t& textureId);
    void setSelectedTextureId(const uint8_t& selectedTexture);
    void draw(std::shared_ptr<GraphicsContext> graphicsContext);

    void setSelected(bool selected);
    bool isSelected(void) const;

    Stats getStats(void) const;
    const float getSpeed(void);
    int getCurrentHP(void) const;
    void takeDamage(const int& amount);
    void attack(std::shared_ptr<Entity> target, std::shared_ptr<Weapon> weapon);

    std::vector<std::shared_ptr<Weapon>> getWeapons(void) const;
    std::shared_ptr<Weapon> addWeapon(std::shared_ptr<Weapon> weapon);
    void removeWeapon(const std::string& name);
    void setCurrentWeapon(std::shared_ptr<Weapon> weapon);
    std::shared_ptr<Weapon> getCurrentWeapon(void);

    std::string getName(void) const;

    glm::ivec2 getPosition(void) const;
    bool isOnTile(const int& x, const int& y);

    void setPosition(const glm::ivec2& position);
    bool findPath(const glm::ivec2& target, const int& stopShortSteps = 0);
    bool isNeighbour(std::shared_ptr<Entity> entity) const;

    int getMovesLeft(void) const;
    bool isTurnInProgress(void) const;
    void useMoves(const int& numMoves);

    void setParticipantId(int participantId);
    int getParticipantId(void) const;

    void reset(void);
    virtual void nextTurn(void);
    virtual bool endTurnCondition(void) = 0;
};
