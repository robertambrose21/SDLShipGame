#include "areaofeffectpool.h"

AreaOfEffectPool::AreaOfEffectPool() : 
    initialised(false)
{
    loadAoeDefinitions();
}

void AreaOfEffectPool::initialise(ApplicationContext& context) {
    this->context = &context;

    context.getTurnController()->addOnNextTurnFunction([&](auto const& currentParticipantId, auto const& turnNumber) {
        for(auto&& [_, aoe]  : aoeObjects) {
            aoe->onNextTurn(currentParticipantId, turnNumber);
        }
    });

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

void AreaOfEffectPool::add(const std::string& name, int ownerId, int turnNumber, const glm::ivec2& position, bool isAnimationOnly) {
    game_assert(initialised);
    game_assert(aoeDefinitions.contains(name));

    auto const& definition = aoeDefinitions[name];
    auto damageSource = DamageSource::parse(definition.damageSource, definition.power);

    aoeObjects.push_back(
        std::make_pair(context->getTurnController()->getTurnNumber(),
            std::make_unique<AreaOfEffect>(
                context->getGrid(),
                context->getEntityPool(),
                *this,
                definition.textureId,
                ownerId,
                turnNumber,
                isAnimationOnly,
                position,
                damageSource,
                Stats::AoEStats { damageSource.getStats(), definition.radius, definition.turns }
            )
        )
    );
}

void AreaOfEffectPool::update(int64_t timeSinceLastFrame) {
    game_assert(initialised);

    for(auto const& index : aoeObjectsForDeletion) {
        aoeObjects.erase(aoeObjects.begin() + index);
    }
    
    aoeObjectsForDeletion.clear();

    for(auto i = 0; i < aoeObjects.size(); i++) {
        auto&& [startTurn, areaOfEffect] = aoeObjects[i];

        areaOfEffect->update(timeSinceLastFrame);

        if(context->getTurnController()->getTurnNumber() - startTurn >= areaOfEffect->getStats().duration) {
            aoeObjectsForDeletion.push_back(i);
        }
    }
}

std::vector<AreaOfEffect*> AreaOfEffectPool::getAoeEffects(void) {
    game_assert(initialised);

    // TODO: just return the map
    std::vector<AreaOfEffect*> aoes;

    for(auto&& [_, aoe] : aoeObjects) {
        aoes.push_back(aoe.get());
    }

    return aoes;
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