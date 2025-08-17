#pragma once

#include <vector>
#include <optional>

#include "core/glmimport.h"
#include "core/util/idgenerator.h"
#include "core/components/factioned.h"
#include "game/effects/effecttypes.h"
#include "game/stats/stats.h"
#include "game/data/tiles.h"

class Entity;
class Item;
class AreaOfEffect;
class Weapon;
class Projectile;
class Grid;

struct ItemEventData {
    enum Type {
        SPAWN,
        REMOVED
    };

    std::string droppedBy;
    std::vector<Item*> items;
    Type type;
};

struct AreaOfEffectEventData {
    AreaOfEffect* aoe;
    Entity* target;
    int damage;
};

struct ProjectileEventData {
    Projectile* projectile;
    Entity* target;
    glm::ivec2 hitPosition;
    int damage;
};

struct MeleeWeaponEventData {
    Entity* owner;
    Entity* target;
    Weapon* weapon;
    int damage;
};

// -- Actions -----------------------------------
struct ActionEventData {
    std::optional<int> turnNumber;
};

struct MoveActionEventData : public ActionEventData {
    Entity* entity;
    glm::ivec2 position;
    int shortStopSteps;
};

struct AttackActionEventData : public ActionEventData {
    Entity* owner;
    glm::ivec2 target;
    Weapon* weapon;
};

struct TakeItemActionEventData : public ActionEventData {
    Entity* entity;
    std::vector<Item*> items;
};

struct EquipItemActionEventData : public ActionEventData {
    Entity* entity;
    Item* item;
    int slot;
    bool isUnequip;
};

struct EquipWeaponActionEventData : public ActionEventData {
    Entity* entity;
    Item* item;
    UUID weaponId;
};
// ----------------------------------------------

enum DamageType {
    AOE,
    PROJECTILE,
    MELEE
};

struct ApplyDamageEventData {
    int participantId;
    Entity* target;
    DamageType source;
    int damage;
};

// -- Engagements -------------------------------
struct CreateEngagementEventData {
    uint32_t engagementId;
    std::vector<int> participants;
};

struct AddToEngagementEventData {
    uint32_t engagementId;
    int participantId;
};

struct DisengageEventData {
    uint32_t engagementId;
    int participantId;
};

struct RemoveEngagementEventData {
    uint32_t engagementId;
};

struct MergeEngagementEventData {
    uint32_t newEngagementId;
    uint32_t engagementIdA;
    uint32_t engagementIdB;
    std::vector<int> participants;
};
// ----------------------------------------------

// -- Factions ----------------------------------
struct CreateFactionEventData {
    uint32_t id;
    std::string name;
};

struct SetFactionEventData {
    int participantId;
    uint32_t factionId;
};

struct AddFactionEventData {
    int participantId;
    uint32_t factionId;
    Factioned::Faction::Alignment alignment;
};

struct ChangeFactionAlignmentEventData {
    int participantId;
    uint32_t factionId;
    Factioned::Faction::Alignment existingAlignment;
    Factioned::Faction::Alignment newAlignment;
};
// ----------------------------------------------

struct TileEventData {
    int x, y;
    int id;
    bool isWalkable;
    bool isFrozen;
};

struct EntityEffectEvent {
    EffectType type;
    Entity* target;
    int participantId;
    Stats::EffectStats stats;
};

struct GridEffectEvent {
    EffectType type;
    int participantId;
    int x, y;
    int duration;
};

struct GridDirtyEventData {
    bool isGridDirty;
};

struct TilesRevealedEventData {
    int participantId;
    std::vector<RevealedTile> tiles;
};

struct EntitySetPositionEventData {
    Entity* entity;
    glm::ivec2 position;
};

struct EntityVisibilityToParticipantData {
    Entity* entity;
    int visibleToParticipantId;
    bool isVisible;
};

struct EntityUpdateStatsEventData {
    Entity* entity;
};
