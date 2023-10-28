#pragma once

#include <string>

#include "core/util/idgenerator.h"

class Item {
private:
    uint32_t id;
    std::string name;

public:
    Item(const std::string& name);
    Item(const std::string& name, uint32_t id);

    uint32_t getId(void) const;

    std::string getName(void) const;
};