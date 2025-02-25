#include "item.h"

// TODO: Derive from Equippable
const std::set<std::string> Item::EquippableItemTypes = {
    "Helm",
    "Body",
    "Hands",
    "Feet"
};

const std::string Item::UnknownOwner = "Unknown";

Item::Item(
    const std::string& name,
    const std::string& droppedBy,
    Rarity rarity, 
    const Stats::ItemStats& stats, 
    const std::string& type, 
    const glm::ivec2& position
) :
    Item(name, droppedBy, rarity, stats, type, position, getNewId())    
{ }

Item::Item(
    const std::string& name,
    const std::string& droppedBy,
    Rarity rarity, 
    const Stats::ItemStats& stats, 
    const std::string& type, 
    const glm::ivec2& position, 
    uint32_t id
) :
    name(name),
    droppedBy(droppedBy),
    type(type),
    rarity(rarity),
    stats(stats),
    position(position),
    id(id),
    participantId(-1)
{ } 

uint32_t Item::getId(void) const {
    return id;
}

void Item::setId(uint32_t id) {
    this->id = id;
}

std::string Item::getName(void) const {
    return name;
}

std::string Item::getType(void) const {
    return type;
}

std::string Item::getDroppedBy(void) const {
    return droppedBy;
}

Item::Rarity Item::getRarity(void) const {
    return rarity;
}

Stats::ItemStats Item::getStats(void) const {
    return stats;
}

void Item::setWeaponStats(const Stats::WeaponStats& weaponStats) {
    stats.weapon = weaponStats;
}

void Item::setGearStats(const Stats::GearStats& gearStats) {
    stats.gear = gearStats;
}

void Item::setTextureId(uint32_t textureId) {
    this->textureId = textureId;
}

uint32_t Item::getTextureId(void) const {
    return textureId;
}

void Item::setColour(const Colour& colour) {
    this->colour = colour;
}

Item::Colour Item::getColour(void) const {
    return colour;
}

void Item::setPosition(const glm::ivec2& position) {
    this->position = position;
}

glm::ivec2 Item::getPosition(void) const {
    return position;
}

int Item::getParticipantId(void) const {
    return participantId;
}

void Item::setParticipantId(int participantId) {
    this->participantId = participantId;
}

bool Item::isEquippable(void) {
    return EquippableItemTypes.contains(getType());
}

bool Item::isWeapon(void) {
    return type == "Weapon";
}