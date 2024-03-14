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
                AreaOfEffectStats(definition.radius, definition.turns, definition.power),
                DamageSource::parse(definition.damageSource, definition.power)
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

        if(context->getTurnController()->getTurnNumber() - startTurn >= areaOfEffect->getStats().turns) {
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

AreaOfEffectStats AreaOfEffectPool::getStatsFor(const std::string& key) {
    if(!aoeDefinitions.contains(key)) {
        return AreaOfEffectStats();
    }

    auto definition = aoeDefinitions[key];

    return AreaOfEffectStats(
        definition.radius,
        definition.turns,
        definition.power
    );
}