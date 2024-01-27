#include "item.h"

Item::Item(const std::string& name, Rarity rarity, const std::string& type, const glm::ivec2& position) :
    Item(name, rarity, type, position, getNewId())    
{ }

Item::Item(const std::string& name, Rarity rarity, const std::string& type, const glm::ivec2& position, uint32_t id) :
    name(name),
    type(type),
    rarity(rarity),
    position(position),
    id(id),
    participantId(-1)
{ }

uint32_t Item::getId(void) const {
    return id;
}

std::string Item::getName(void) const {
    return name;
}

std::string Item::getType(void) const {
    return type;
}

Item::Rarity Item::getRarity(void) const {
    return rarity;
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