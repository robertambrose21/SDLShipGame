#include "areaofeffectpool.h"

AreaOfEffectPool::AreaOfEffectPool() : 
    initialised(false)
{
    loadAoeDefinitions();
}

void AreaOfEffectPool::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

void AreaOfEffectPool::loadAoeDefinitions(void) {
    std::string directory = "../assets/data/areaofeffects";

    for(const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::ifstream f(entry.path());
        json data = json::parse(f);

        AoeDefinition definition;
        definition.filename = entry.path();
        definition.name = data["name"].get<std::string>();
        definition.textureId = data["textureId"].get<uint32_t>();
        definition.radius = data["radius"].get<float>();
        definition.turns = data["turns"].get<int>();
        definition.damageSource = data["damagePerTurn"].get<std::string>();
        definition.power = data["power"].get<int>();

        std::cout << "Loaded area of effect definition \"" << definition.name << "\"" << std::endl;

        aoeDefinitions[definition.name] = definition;
    }

    game_assert(!aoeDefinitions.empty());
}

void AreaOfEffectPool::add(const std::string& name, int ownerId, const glm::ivec2& position, bool isAnimationOnly) {
    game_assert(initialised);
    game_assert(aoeDefinitions.contains(name));

    auto const& definition = aoeDefinitions[name];
    auto damageSource = DamageSource::parse(definition.damageSource, definition.power);

    auto areaOfEffect = std::make_unique<AreaOfEffect>(
        context->getGrid(),
        context->getEntityPool(),
        *this,
        definition.textureId,
        ownerId,
        isAnimationOnly,
        position,
        damageSource,
        Stats::AoEStats { damageSource.getStats(), definition.radius, definition.turns }
    );

    auto participant = context->getTurnController()->getParticipant(ownerId);
    if(participant->hasAnyEngagement()) {
        auto engagement = participant->getEngagement();

        if(!engagementAoEs.contains(engagement->getId())) {
            engagement->addOnNextTurnWorker([&](int currentParticipantId, int turnNumber, uint32_t engagementId) {
                for(auto const& aoe : engagementAoEs[engagementId]) {
                    aoe->onNextTurn(currentParticipantId, turnNumber);
                }
            });
        }

        spdlog::trace("Adding AoE to engagement {} from participant {}", engagement->getId(), ownerId);
        engagementAoEs[engagement->getId()].push_back(std::move(areaOfEffect));
    }
    else {
        adhocAoEs.push_back(std::move(areaOfEffect));
    }
}

void AreaOfEffectPool::update(int64_t timeSinceLastFrame) {
    game_assert(initialised);

    // TODO: Move different AoE handling to separate methods
    // Engagement AoEs
    auto engagementController = context->getTurnController()->getEngagementController();

    // Remove dead engagements
    std::erase_if(engagementAoEs, [&](const auto& engagementAoE) {
        auto& [engagementId, _] = engagementAoE;
        return !engagementController->hasEngagement(engagementId);
    });

    for(auto& [engagementId, areaOfEffects] : engagementAoEs) {
        auto engagement = engagementController->getEngagement(engagementId);

        // Shouldn't happen
        if(engagement == nullptr) {
            spdlog::critical(
                "Cannot process {} AoEs for engagement {}, engagement does not exist", 
                areaOfEffects.size(), 
                engagementId
            );
            game_assert(false);
            continue;
        }

        std::erase_if(areaOfEffects, [](const auto& areaOfEffect) {
            return areaOfEffect->isComplete();
        });

        for(auto const& areaOfEffect : areaOfEffects) {
            areaOfEffect->update(timeSinceLastFrame);
        }
    }

    // Adhoc AoEs
    std::erase_if(adhocAoEs, [](const auto& areaOfEffect) {
        return areaOfEffect->isComplete();
    });

    for(auto& areaOfEffect : adhocAoEs) {
        // TODO: Make these timed and not execute on every update tick
        areaOfEffect->onNextTurn(areaOfEffect->getOwnerId(), -1);
        areaOfEffect->update(timeSinceLastFrame);
    }
}

// std::vector<AreaOfEffect*> AreaOfEffectPool::getAoeEffects(void) {
//     game_assert(initialised);

//     // TODO: just return the map
//     std::vector<AreaOfEffect*> aoes;

//     // for(auto&& [_, aoe] : aoeObjects) {
//     //     aoes.push_back(aoe.get());
//     // }

//     return aoes;
// }

const std::map<uint32_t, std::vector<std::unique_ptr<AreaOfEffect>>>& AreaOfEffectPool::getEngagementAoEs(void) const {
    return engagementAoEs;
}

const std::vector<std::unique_ptr<AreaOfEffect>>& AreaOfEffectPool::getAdhocAoEs(void) const {
    return adhocAoEs;
}

Stats::AoEStats AreaOfEffectPool::getStatsFor(const std::string& key) {
    if(!aoeDefinitions.contains(key)) {
        return Stats::AoEStats();
    }

    auto definition = aoeDefinitions[key];

    return {
        DamageSource::parse(definition.damageSource, definition.power).getStats(),
        definition.radius,
        definition.turns
    };
}