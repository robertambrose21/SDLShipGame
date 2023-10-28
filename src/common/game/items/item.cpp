#include "item.h"

Item::Item(const std::string& name) :
    Item(name, getNewId())
{ }

Item::Item(const std::string& name, uint32_t id) :
    name(name),
    id(id)
{ }

uint32_t Item::getId(void) const {
    return id;
}

std::string Item::getName(void) const {
    return name;
}