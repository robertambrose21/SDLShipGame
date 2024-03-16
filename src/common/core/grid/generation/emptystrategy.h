#pragma once

#include "generationstrategy.h"

class EmptyStrategy : public GenerationStrategy {
public:
    EmptyStrategy(int width, int height);

    std::vector<std::vector<Grid::Tile>> generate(void);
};