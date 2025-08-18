#include "effectcontroller.h"

EffectController::EffectController() :
    initialised(false)
{ }

void EffectController::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

void EffectController::update(int64_t timeSinceLastFrame) {
    game_assert(initialised);

    updateEngagementEffects(timeSinceLastFrame);
    updateAdhocEffects(timeSinceLastFrame);
}

void EffectController::updateEngagementEffects(int64_t timeSinceLastFrame) {
    auto engagementController = context->getGameController()->getEngagementController();

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
                effect->onEffectEnd();
            }
            return isComplete;
        });

        for(auto& effect : effects) {
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
                effect->onEffectEnd();
            }
            return isComplete;
        });

        for(auto const& effect : effects) {
            effect->update(timeSinceLastFrame);
        }
    }
}

void EffectController::updateAdhocEffects(int64_t timeSinceLastFrame) {    
    std::erase_if(adhocEffects, [](const auto& effect) {
        bool isComplete = effect->getTicksLeft() <= 0;
        if(isComplete) {
            spdlog::trace(
                "Effect for participant {} complete - removing", 
                effect->getOwnerId()
            );
            effect->onEffectEnd();
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
            effect->onEffectEnd();
        }
        return isComplete;
    });

    for(auto& effect : adhocEffects) {
        if(effect->getTimeSinceLastTick() >= Effect::RealTimeTick) {
            effect->apply();
            effect->nextTurn();
        }

        effect->update(timeSinceLastFrame);
    }

    for(auto& effect : adhocGridEffects) {
        if(effect->getTimeSinceLastTick() >= Effect::RealTimeTick) {
            effect->apply();
            effect->nextTurn();
        }

        effect->update(timeSinceLastFrame);
    }
}

Effect* EffectController::addEffect(std::unique_ptr<Effect> effect) {
    auto actorId = effect->getTarget()->getId();
    auto effectPtr = effect.get();

    auto participant = context->getGameController()->getParticipant(effect->getOwnerId());

    if(participant->hasAnyEngagement()) {
        auto engagement = participant->getEngagement();

        if(!engagementEffects.contains(engagement->getId())) {
            engagement->addOnNextTurnWorker([&](int currentParticipantId, int turnNumber, uint32_t engagementId) {
                for(auto const& effect : engagementEffects[engagementId]) {
                    if(effect->getOwnerId() != currentParticipantId) {
                        continue;
                    }

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

    effectPtr->apply();

    publish<ActorEffectEvent>({ 
        effectPtr->getType(), 
        effectPtr->getTarget(), 
        effectPtr->getOwnerId(),
        effectPtr->getStats()
    });

    return effectPtr;
}

GridEffect* EffectController::addGridEffect(std::unique_ptr<GridEffect> effect) {
    auto effectPtr = effect.get();

    auto participant = context->getGameController()->getParticipant(effect->getOwnerId());

    if(participant->hasAnyEngagement()) {
        auto engagement = participant->getEngagement();

        if(!engagementGridEffects.contains(engagement->getId())) {
            engagement->addOnNextTurnWorker([&](int currentParticipantId, int turnNumber, uint32_t engagementId) {
                for(auto const& effect : engagementGridEffects[engagementId]) {
                    if(effect->getOwnerId() != currentParticipantId) {
                        continue;
                    }

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