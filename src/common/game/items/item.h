#pragma once

#include <string>
#include <vector>
#include <set>

#include "core/util/idgenerator.h"
#include "core/glmimport.h"
#include "game/event/events.h"

// TODO: This might not have a position - should probably make different classes for dropped items/equipped items?
class Item {
public:
    // TODO: Come up with names which don't just copy WoW
    enum Rarity {
        Unknown,
        Junk,
        Common,
        Uncommon,
        Rare,
        Epic
    };

    static const std::set<std::string> EquippableItemTypes;

private:
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

    std::string name;
    std::string type;
    Rarity rarity;

    uint32_t id;
    uint32_t textureId;
    Colour colour;
    glm::ivec2 position;

    int participantId;

public:
    Item(const std::string& name, Rarity rarity, const std::string& type, const glm::ivec2& position);
    Item(const std::string& name, Rarity rarity, const std::string& type, const glm::ivec2& position, uint32_t id);

    std::string getName(void) const;
    std::string getType(void) const;
    Rarity getRarity(void) const;

    uint32_t getId(void) const;

    void setTextureId(uint32_t textureId);
    uint32_t getTextureId(void) const;
    void setColour(const Colour& colour);
    Colour getColour(void) const;
    void setPosition(const glm::ivec2& position);
    glm::ivec2 getPosition(void) const;

    int getParticipantId(void) const;
    void setParticipantId(int participantId);

    bool isEquippable(void);
};
