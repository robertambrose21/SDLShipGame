#pragma once

#include <map>
#include <vector>

#include "effect.h"

class EffectController {
private:
    std::map<uint32_t, std::vector<std::unique_ptr<Effect>>> effects; // vector of effects

public:
    EffectController();

    void onNextTurn(void);

    Effect* addEffect(std::unique_ptr<Effect> effect);
};
