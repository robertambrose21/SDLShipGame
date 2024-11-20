#pragma once

#include <string>
#include <format>

#include "core/glmimport.h"
#include "core/util/idgenerator.h"
#include "game/weapons/weapon.h"
#include "core/util/gameassert.h"
#include "core/event/eventpublisher.h"
#include "core/grid/grid.h"
#include "game/application/applicationcontext.h"
#include "game/effects/effect.h"
#include "game/stats/stats.h"
#include "game/items/gear.h"

// TODO: Fix with modules?
class Weapon;
class Entity;
class Action;
class Effect;
class ApplicationContext;

struct EntityEventData {
    Entity* entity;
    std::string type;
};

class Entity {
public:
    // TODO: Move this out of entity class
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
    bool engaged;

    Grid* grid;
    EventPublisher<EntityEventData, EntitySetPositionEventData, EntityUpdateStatsEventData>& publisher;

    glm::ivec2 position;
    std::deque<glm::ivec2> path;
    uint32_t timeSinceLastMoved;

    Stats::EntityStats baseStats;
    Stats::EntityStats stats;
    std::map<Equippable<Stats::GearStats>::Slot, std::unique_ptr<Gear>> equippedGear;

    std::map<UUID, std::unique_ptr<Weapon>> weapons;
    Weapon* currentWeapon;

    std::map<int, std::deque<std::unique_ptr<Action>>> actionsChain;
    std::map<int, std::deque<Action*>> externalActionsChain;
    bool externalActionsChainNeedsRecalculating;

    std::string name;

    int participantId;

    bool isFrozen;
    bool isPoisoned;

    void clearAllActions(void);

public:
    const uint32_t MOVES_PER_SECOND = 5;

    Entity(
        Grid* grid,
        uint32_t id,
        EventPublisher<EntityEventData, EntitySetPositionEventData, EntityUpdateStatsEventData>& publisher,
        const std::string& name,
        const Stats::EntityStats& stats
    );

    Entity(
        Grid* grid,
        EventPublisher<EntityEventData, EntitySetPositionEventData, EntityUpdateStatsEventData>& publisher,
        const std::string& name,
        const Stats::EntityStats& stats
    );

    // TODO: Should these be in EntityPool?
    static Entity* filterByTile(
        int x, 
        int y, 
        const std::set<Entity*>& entities
    ) {
        for(auto const& entity : entities) {
            if(entity->isOnTile(x, y)) {
                return entity;
            }
        }

        return nullptr;
    }

    static Entity* filterByTile(
        int x, 
        int y, 
        const std::vector<Entity*>& entities,
        int excludedParticipantId = -1
    ) {
        for(auto entity : entities) {
            if(entity->isOnTile(x, y) && entity->getParticipantId() != excludedParticipantId) {
                return entity;
            }
        }

        return nullptr;
    }

    static std::vector<Entity*> filterByTiles(
        const std::vector<glm::ivec2>& tiles,
        const std::vector<Entity*>& entities,
        int excludedParticipantId = -1
    ) {
        std::vector<Entity*> filteredEntities;

        for(auto entity : entities) {
            for(auto const& tile : tiles) {
                if(entity->isOnTile(tile.x, tile.y) && entity->getParticipantId() != excludedParticipantId) {
                    filteredEntities.push_back(entity);
                }
            }
        }

        return filteredEntities;
    }

    void update(int64_t timeSinceLastFrame, bool& quit);

    void setTextureId(uint32_t textureId);
    void setSelectedTextureId(uint32_t selectedTexture);
    uint32_t getTextureId(void) const;
    uint32_t getSelectedTextureId(void) const;

    void setColour(const Colour& colour);
    Colour getColour(void) const;

    void setSelected(bool selected);
    bool isSelected(void) const;

    void engage(void);
    void disengage(void);
    bool isEngaged(void) const;

    Stats::EntityStats getStats(void) const;

    void setGear(std::unique_ptr<Gear> gear);
    void removeGear(Equippable<Stats::GearStats>::Slot slot);
    Gear* getGear(Equippable<Stats::GearStats>::Slot slot);

    void applyStats();

    const float getSpeed(void);
    int getCurrentHP(void) const;
    void setCurrentHP(uint32_t hp);
    void takeDamage(uint32_t amount);
    void attack(const glm::ivec2& target, const UUID& weaponId, bool isAnimationOnly = false);

    std::vector<Weapon*> getWeapons(void) const;
    Weapon* getWeapon(const UUID& weaponId);
    bool hasWeapon(const UUID& weaponId);
    Weapon* addWeapon(std::unique_ptr<Weapon> weapon);
    void removeWeapon(const UUID& weaponId);
    void removeAllWeapons(void);
    void setCurrentWeapon(const UUID& weaponId);
    Weapon* getCurrentWeapon(void);
    
    uint32_t getId(void) const;
    void setId(uint32_t id);
    std::string getName(void) const;
    void setName(const std::string& name);

    glm::ivec2 getPosition(void) const;
    bool isOnTile(int x, int y);

    void setPosition(const glm::ivec2& position);
    int findPath(const glm::ivec2& target, int stopShortSteps = 0);
    std::deque<glm::ivec2> calculatePath(const glm::ivec2& target, int stopShortSteps = 0);
    void setPath(const std::deque<glm::ivec2>& path);
    bool isNeighbour(Entity* entity) const;
    bool hasPath(void);

    int getMovesLeft(void) const;
    void setMovesLeft(int movesLeft);
    int getAggroRange(void) const;
    int getDisengagementRange(void) const;
    bool isTurnInProgress(void);
    bool hasAnimationsInProgress(void);
    void useMoves(int numMoves);

    bool queueAction(
        ApplicationContext* context,
        std::unique_ptr<Action> action, 
        std::function<void(Action&)> onSuccessfulQueue, 
        bool skipValidation = false
    );
    std::deque<Action*>& getActionsChain(int turnNumber);
    void recalculateActionsChain();
    void popAction(int currentTurnNumber);

    void setParticipantId(int participantId);
    int getParticipantId(void) const;
    bool hasParticipant(void) const;

    void nextTurn(void);
    void endTurn(void);
    void reset(void);
    
    bool getIsFrozen(void) const;
    void setFrozen(bool isFrozen);

    bool getIsPoisoned(void) const;
    void setIsPoisoned(bool isPoisoned);
};
