#include "effectcontroller.h"

EffectController::EffectController() :
    initialised(false)
{ }

void EffectController::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

// void EffectController::onNextTurn(void) {
//     // // Entity effects
//     // for(auto& [_, entityEffects] : effects) {
//     //     std::erase_if(entityEffects, [&](const auto& effect) {
//     //         return effect->getTicksLeft() <= 0;
//     //     });

//     //     for(auto& effect : entityEffects) {
//     //         effect->apply();
//     //         effect->nextTurn();
//     //     }
//     // }

//     // // Grid effects
//     // std::erase_if(gridEffects, [&](const auto& effect) {
//     //     return effect->getDuration() <= 0;
//     // });

//     // for(auto& effect : gridEffects) {
//     //     effect->apply();
//     //     effect->nextTurn();
//     // }
// }

// TODO: Divide this up
void EffectController::update(int64_t timeSinceLastFrame) {
    game_assert(initialised);

    auto engagementController = context->getTurnController()->getEngagementController();

    std::erase_if(engagementEffects, [&](const auto& engagementEffect) {
        auto& [engagementId, _] = engagementEffect;
        return !engagementController->hasEngagement(engagementId);
    });

    std::erase_if(engagementGridEffects, [&](const auto& engagementEffect) {
        auto& [engagementId, _] = engagementEffect;
        return !engagementController->hasEngagement(engagementId);
    });

    for(auto& [engagementId, effects]: engagementEffects) {
        std::erase_if(effects, [](const auto& effect) {
            bool isComplete = effect->getTicksLeft() <= 0;
            if(isComplete) {
                spdlog::trace(
                    "Effect for participant {} complete - removing", 
                    effect->getOwnerId()
                );
            }
            return isComplete;
        });

        for(auto const& effect : effects) {
            effect->update(timeSinceLastFrame);
        }
    }

    for(auto& [engagementId, effects]: engagementGridEffects) {
        std::erase_if(effects, [](const auto& effect) {
            bool isComplete = effect->getDuration() <= 0;
            if(isComplete) {
                spdlog::trace(
                    "GridEffect for participant {} complete - removing", 
                    effect->getOwnerId()
                );
            }
            return isComplete;
        });

        for(auto const& effect : effects) {
            effect->update(timeSinceLastFrame);
        }
    }

    std::erase_if(adhocEffects, [](const auto& effect) {
        bool isComplete = effect->getTicksLeft() <= 0;
        if(isComplete) {
            spdlog::trace(
                "Effect for participant {} complete - removing", 
                effect->getOwnerId()
            );
        }
        return isComplete;
    });

    std::erase_if(adhocGridEffects, [](const auto& effect) {
        bool isComplete = effect->getDuration() <= 0;
        if(isComplete) {
            spdlog::trace(
                "GridEffect for participant {} complete - removing", 
                effect->getOwnerId()
            );
        }
        return isComplete;
    });

    for(auto const& effect : adhocEffects) {
        if(effect->getTimeSinceLastTick() >= Effect::RealTimeTick) {
            effect->apply();
            effect->nextTurn();
        }

        effect->update(timeSinceLastFrame);
    }

    for(auto const& effect : adhocGridEffects) {
        if(effect->getTimeSinceLastTick() >= Effect::RealTimeTick) {
            effect->apply();
            effect->nextTurn();
        }

        effect->update(timeSinceLastFrame);
    }
}

Effect* EffectController::addEffect(std::unique_ptr<Effect> effect) {
    auto entityId = effect->getTarget()->getId();
    auto effectPtr = effect.get();

    auto participant = context->getTurnController()->getParticipant(effect->getOwnerId());

    if(participant->hasAnyEngagement()) {
        auto engagement = participant->getEngagement();

        if(!engagementEffects.contains(engagement->getId())) {
            engagement->addOnNextTurnWorker([&](int currentParticipantId, int turnNumber, uint32_t engagementId) {
                for(auto const& effect : engagementEffects[engagementId]) {
                    effect->apply();
                    effect->nextTurn();
                }
            });
        }

        spdlog::trace("Adding Effect to engagement {} from participant {}", engagement->getId(), effect->getOwnerId());
        engagementEffects[engagement->getId()].push_back(std::move(effect));
    }
    else {
        adhocEffects.push_back(std::move(effect));
    }

    // effects[entityId].push_back(std::move(effect));
    effectPtr->apply();

    publish<EntityEffectEvent>({ 
        effectPtr->getType(), 
        effectPtr->getTarget(), 
        effectPtr->getOwnerId(),
        effectPtr->getStats()
    });

    return effectPtr;
}

GridEffect* EffectController::addGridEffect(std::unique_ptr<GridEffect> effect) {
    auto effectPtr = effect.get();
    // gridEffects.push_back(std::move(effect));

    auto participant = context->getTurnController()->getParticipant(effect->getOwnerId());

    if(participant->hasAnyEngagement()) {
        auto engagement = participant->getEngagement();

        if(!engagementGridEffects.contains(engagement->getId())) {
            engagement->addOnNextTurnWorker([&](int currentParticipantId, int turnNumber, uint32_t engagementId) {
                for(auto const& effect : engagementGridEffects[engagementId]) {
                    effect->apply();
                    effect->nextTurn();
                }
            });
        }

        spdlog::trace("Adding GridEffect to engagement {} from participant {}", engagement->getId(), effect->getOwnerId());
        engagementGridEffects[engagement->getId()].push_back(std::move(effect));
    }
    else {
        adhocGridEffects.push_back(std::move(effect));
    }

    effectPtr->apply();

    publish<GridEffectEvent>({ 
        effectPtr->getType(), 
        effectPtr->getOwnerId(),
        effectPtr->getX(), 
        effectPtr->getY(), 
        effectPtr->getDuration()
    });

    return effectPtr;
}