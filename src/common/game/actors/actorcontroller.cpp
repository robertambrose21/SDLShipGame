#include "actorcontroller.h"

void ActorController::initialise(ApplicationContext& context) {
    this->context = &context;
}

void ActorController::applyDamage(entt::entity entity, uint32_t amount) {
    auto& stats = context->getEntityRegistry().get<Stats::ActorStats>(entity);

    if(amount >= stats.hp) {
        stats.hp = 0;
    }
    else {
        stats.hp -= amount;
    }

    publish<ActorUpdateStatsEventData>({ entity });
}

void ActorController::applyStats(entt::entity entity) {
    auto& actor = context->getEntityRegistry().get<Actor>(entity);
    auto& stats = context->getEntityRegistry().get<Stats::ActorStats>(entity);

    uint32_t currentHp = stats.hp;
    stats = actor.getBaseStats();
    stats.hp = currentHp;

    for(auto const& [slot, gear] : actor.getEquippedGear()) {
        if(gear != nullptr) {
            gear->addTo(stats);
        }
    }

    for(auto weapon : actor.getWeapons()) {
        if(weapon != nullptr) {
            weapon->addTo(stats);
        }
    }

    publish<ActorUpdateStatsEventData>({ entity });
}

void ActorController::useMoves(entt::entity entity, int numMoves) {
    auto& stats = context->getEntityRegistry().get<Stats::ActorStats>(entity);
    auto& actor = context->getEntityRegistry().get<Actor>(entity);

    stats.movesLeft -= numMoves;
    
    if(stats.movesLeft <= 0) {
        stats.movesLeft = 0;
        actor.setPath({});
    }

    publish<ActorUpdateStatsEventData>({ entity });
}

void ActorController::nextTurn(entt::entity entity) {
    reset(entity);
}

void ActorController::endTurn(entt::entity entity) {
    auto& stats = context->getEntityRegistry().get<Stats::ActorStats>(entity);
    auto& actor = context->getEntityRegistry().get<Actor>(entity);

    stats.movesLeft = 0;
    actor.setPath({});

    for(auto weapon : actor.getWeapons()) {
        weapon->setUsesLeft(0);
    }

    publish<ActorUpdateStatsEventData>({ entity });
}

bool ActorController::isTurnInProgress(entt::entity entity) {
    auto& stats = context->getEntityRegistry().get<Stats::ActorStats>(entity);
    auto& actor = context->getEntityRegistry().get<Actor>(entity);

    return (actor.getCurrentWeapon() != nullptr && !actor.getCurrentWeapon()->hasFinished()) || stats.movesLeft > 0;
}

void ActorController::reset(entt::entity entity) {
    auto& stats = context->getEntityRegistry().get<Stats::ActorStats>(entity);
    auto& actor = context->getEntityRegistry().get<Actor>(entity);

    stats.movesLeft = stats.movesPerTurn;
    actor.setPath({});

    actor.setFrozen(false);
    actor.setIsPoisoned(false);

    for(auto weapon : actor.getWeapons()) {
        if(weapon != nullptr) {
            weapon->reset();
        }
    }

    publish<ActorUpdateStatsEventData>({ entity });
}

void ActorController::engage(entt::entity entity) {
    auto& actor = context->getEntityRegistry().get<Actor>(entity);

    if(actor.isEngaged()) {
        return;
    }

    actor.setEngaged(true);
    actor.clearAllActions();

    reset(entity);
}

void ActorController::disengage(entt::entity entity) {
    auto& actor = context->getEntityRegistry().get<Actor>(entity);

    if(!actor.isEngaged()) {
        return;
    }

    actor.setEngaged(false);
    actor.clearAllActions();

    reset(entity);
}

float ActorController::getSpeed(entt::entity entity) {
    auto& stats = context->getEntityRegistry().get<Stats::ActorStats>(entity);
    return 2000.0f / (5 * stats.movesPerTurn);
}
