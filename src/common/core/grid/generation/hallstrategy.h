#pragma once

#include "core/util/randomrolls.h"
#include "generationstrategy.h"

class HallStrategy : public GenerationStrategy {
private:
    int maxTunnels;
    int maxTunnelLength;

public:
    HallStrategy(int width, int height, int maxTunnels, int maxTunnelLength);

    std::vector<std::vector<Grid::Tile>> generate(void);
};