#pragma once

#include <string>

#include "core/glmimport.h"
#include "core/util/idgenerator.h"
#include "graphics/gridrenderer.h"
#include "graphics/healthbar.h"
#include "game/weapons/weapon.h"
#include "core/util/gameassert.h"
#include "entitystats.h"
#include "game/effects/statuseffect.h"

// TODO: Fix with modules?
class Weapon;

class Entity {
public:
    typedef struct _colour {
        uint8_t r, g, b, a;

        _colour() :
            r(0xFF),
            g(0xFF),
            b(0xFF),
            a(0xFF)
        { }

        _colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
            r(r),
            g(g),
            b(b),
            a(a)
        { }
    } Colour;

private: 
    uint32_t id;
    uint32_t textureId;
    uint32_t selectedTextureId;
    Colour colour;

    bool selected;

    std::shared_ptr<Grid> grid;
    std::shared_ptr<HealthBar> healthBar;

    glm::ivec2 position;
    std::deque<glm::ivec2> path;
    uint32_t timeSinceLastMoved;

    EntityBaseStats stats;
    EntityCurrentStats currentStats;
    std::set<StatusEffect> statusEffects;

    std::map<uint32_t, std::shared_ptr<Weapon>> weapons;
    std::shared_ptr<Weapon> currentWeapon;

    std::string name;

    int participantId;

    int frozenForNumTurns;

public:
    const uint32_t MOVES_PER_SECOND = 5;

    Entity(
        uint32_t id,
        const std::string& name,
        const EntityBaseStats& stats
    );

    Entity(
        const std::string& name,
        const EntityBaseStats& stats
    );

    // TODO: Should these be in EntityPool?
    static std::shared_ptr<Entity> filterByTile(
        int x, 
        int y, 
        const std::set<std::shared_ptr<Entity>>& entities
    ) {
        for(auto const& entity : entities) {
            if(entity->isOnTile(x, y)) {
                return entity;
            }
        }

        return nullptr;
    }

    static std::shared_ptr<Entity> filterByTile(
        int x, 
        int y, 
        const std::map<uint32_t, std::shared_ptr<Entity>>& entities,
        int excludedParticipantId = -1
    ) {
        for(auto [entityId, entity] : entities) {
            if(entity->isOnTile(x, y) && entity->getParticipantId() != excludedParticipantId) {
                return entity;
            }
        }

        return nullptr;
    }

    static std::set<std::shared_ptr<Entity>> filterByTiles(
        const std::vector<glm::ivec2>& tiles,
        const std::map<uint32_t, std::shared_ptr<Entity>>& entities,
        int excludedParticipantId = -1
    ) {
        std::set<std::shared_ptr<Entity>> filteredEntities;

        for(auto [entityId, entity] : entities) {
            for(auto const& tile : tiles) {
                if(entity->isOnTile(tile.x, tile.y) && entity->getParticipantId() != excludedParticipantId) {
                    filteredEntities.insert(entity);
                }
            }
        }

        return filteredEntities;
    }

    void update(uint32_t timeSinceLastFrame, bool& quit);

    void setTextureId(uint32_t textureId);
    void setSelectedTextureId(uint32_t selectedTexture);
    uint32_t getTextureId(void) const;

    void setColour(const Colour& colour);
    Colour getColour(void) const;

    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);

    void setSelected(bool selected);
    bool isSelected(void) const;

    EntityBaseStats getBaseStats(void) const;
    EntityCurrentStats getCurrentStats(void) const;
    const float getSpeed(void);
    int getCurrentHP(void) const;
    void setCurrentHP(int hp);
    void takeDamage(int amount);
    void attack(const glm::ivec2& target, const std::shared_ptr<Weapon>& weapon);

    std::map<uint32_t, std::shared_ptr<Weapon>> getWeapons(void) const;
    std::shared_ptr<Weapon> getWeapon(uint32_t weaponId);
    bool hasWeapon(uint32_t weaponId);
    std::shared_ptr<Weapon> addWeapon(const std::shared_ptr<Weapon>& weapon);
    void removeWeapon(const std::string& name);
    void setCurrentWeapon(const std::shared_ptr<Weapon>& weapon);
    std::shared_ptr<Weapon> getCurrentWeapon(void);

    uint32_t getId(void) const;
    void setId(uint32_t id);
    std::string getName(void) const;
    void setName(const std::string& name);

    glm::ivec2 getPosition(void) const;
    bool isOnTile(int x, int y);

    void setPosition(const glm::ivec2& position);
    int findPath(const glm::ivec2& target, int stopShortSteps = 0);
    bool isNeighbour(const std::shared_ptr<Entity>& entity) const;
    bool hasPath(void);

    int getMovesLeft(void) const;
    void setMovesLeft(int movesLeft);
    bool isTurnInProgress(void) const;
    void useMoves(int numMoves);

    void setParticipantId(int participantId);
    int getParticipantId(void) const;

    void reset(void);
    void nextTurn(void);
    bool endTurnCondition(void);

    bool getIsFrozen(void) const;
    void setFrozenFor(int numTurns);
};
