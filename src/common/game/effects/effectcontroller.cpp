#include "effectcontroller.h"

EffectController::EffectController()
{ }

void EffectController::onNextTurn(void) {
    // Entity effects
    for(auto& [_, entityEffects] : effects) {
        std::erase_if(entityEffects, [&](const auto& effect) {
            return effect->getTicksLeft() <= 0;
        });

        for(auto& effect : entityEffects) {
            effect->apply();
            effect->nextTurn();
        }
    }

    // Grid effects
    std::erase_if(gridEffects, [&](const auto& effect) {
        return effect->getDuration() <= 0;
    });

    for(auto& effect : gridEffects) {
        effect->apply();
        effect->nextTurn();
    }
}

Effect* EffectController::addEffect(std::unique_ptr<Effect> effect) {
    auto entityId = effect->getTarget()->getId();
    auto effectPtr = effect.get();
    effects[entityId].push_back(std::move(effect));
    effectPtr->apply();

    publish<EntityEffectEvent>({ 
        effectPtr->getType(), 
        effectPtr->getTarget(), 
        effectPtr->getStats()
    });

    return effectPtr;
}

GridEffect* EffectController::addGridEffect(std::unique_ptr<GridEffect> effect) {
    auto effectPtr = effect.get();
    gridEffects.push_back(std::move(effect));
    effectPtr->apply();

    publish<GridEffectEvent>({ 
        effectPtr->getType(), 
        effectPtr->getX(), 
        effectPtr->getY(), 
        effectPtr->getDuration()
    });

    return effectPtr;
}