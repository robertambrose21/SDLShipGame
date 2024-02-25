#include "effectcontroller.h"

EffectController::EffectController()
{ }

void EffectController::onNextTurn(void) {
    for(auto& [_, entityEffects] : effects) {
        std::erase_if(entityEffects, [](const auto& effect) {
            return effect->getDuration() <= 0;
        });

        for(auto& effect : entityEffects) {
            effect->apply();
        }
    }
}

Effect* EffectController::addEffect(std::unique_ptr<Effect> effect) {
    auto entityId = effect->getTarget()->getId();
    auto effectPtr = effect.get();
    effects[entityId].push_back(std::move(effect));
    return effectPtr;
}