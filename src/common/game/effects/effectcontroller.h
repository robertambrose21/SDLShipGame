#pragma once

#include <map>
#include <vector>

#include "effect.h"
#include "grideffect.h"

class EffectController {
private:
    std::map<uint32_t, std::vector<std::unique_ptr<Effect>>> effects;
    std::vector<std::unique_ptr<GridEffect>> gridEffects;

public:
    EffectController();

    void onNextTurn(void);

    Effect* addEffect(std::unique_ptr<Effect> effect);
    GridEffect* addGridEffect(std::unique_ptr<GridEffect> effect);
};
