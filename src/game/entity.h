#pragma once

#include <string>

#include "../core/glmimport.h"
#include "../graphics/gridrenderer.h"
#include "weapon.h"

class Weapon;

class Entity {
public:
    typedef struct _stats {
        int movesPerTurn;
        int hp;
    } Stats;

private:
    std::shared_ptr<Texture> texture;

    glm::ivec2 position;
    std::deque<glm::ivec2> path;
    Uint32 timeSinceLastMoved;

    Stats stats;
    std::vector<std::shared_ptr<Weapon>> weapons;

    std::string name;
    int movesLeft;

protected:
    std::shared_ptr<GridRenderer> grid;

    virtual void additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) = 0;

public:
    const Uint32 MOVES_PER_SECOND = 5;

    Entity(
        std::shared_ptr<GridRenderer> grid,
        const std::string& name,
        Stats stats,
        std::vector<std::shared_ptr<Weapon>> weapons
    );

    void update(const Uint32& timeSinceLastFrame, bool& quit);

    void setTexture(std::shared_ptr<Texture> texture);
    void draw(std::shared_ptr<SDL_Renderer> renderer);

    Stats getStats(void) const;
    const float getSpeed(void);
    void takeDamage(const int& amount);
    void attack(std::shared_ptr<Entity> target, std::shared_ptr<Weapon> weapon);

    std::vector<std::shared_ptr<Weapon>> getWeapons(void) const;
    std::shared_ptr<Weapon> addWeapon(std::shared_ptr<Weapon> weapon);
    void removeWeapon(const std::string& name);

    std::string getName(void) const;

    glm::ivec2 getPosition(void) const;
    void setPosition(const glm::ivec2& position);
    bool findPath(const glm::ivec2& target, const int& stopShortSteps = 0);
    bool isNeighbour(std::shared_ptr<Entity> entity) const;

    int getMovesLeft(void) const;
    void useMoves(const int& numMoves);

    void nextTurn(void);

    virtual bool endTurnCondition(void) = 0;
};
