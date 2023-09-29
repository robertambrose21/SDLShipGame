#include "areaofeffectpool.h"

AreaOfEffectPool::AreaOfEffectPool(
    TurnController& turnController, 
    Grid& grid
) :
    turnController(turnController),
    grid(grid)
{
    loadAoeDefinitions();

    turnController.addOnNextTurnFunction([&](auto const& currentParticipant, auto const& turnNumber) {
        for(auto&& [_, aoe]  : aoeObjects) {
            aoe->onNextTurn(currentParticipant, turnNumber);
        }
    });
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
        definition.damagePerTurn = data["damagePerTurn"].get<int>();

        std::cout << "Loaded area of effect definition \"" << definition.name << "\"" << std::endl;

        aoeDefinitions[definition.name] = definition;
    }

    game_assert(!aoeDefinitions.empty());
}

void AreaOfEffectPool::add(const std::string& name, int ownerId, int turnNumber, const glm::ivec2& position) {
    game_assert(aoeDefinitions.contains(name));
    auto const& definition = aoeDefinitions[name];
    aoeObjects.push_back(
        std::make_pair(turnController.getTurnNumber(),
            std::make_unique<AreaOfEffect>(
                grid,
                *this,
                definition.textureId,
                ownerId,
                turnNumber,
                position,
                AreaOfEffect::Stats { definition.radius, definition.turns, definition.damagePerTurn }
            )
        )
    );
}

void AreaOfEffectPool::update(uint32_t timeSinceLastFrame) {
    for(auto const& index : aoeObjectsForDeletion) {
        aoeObjects.erase(aoeObjects.begin() + index);
    }
    
    aoeObjectsForDeletion.clear();

    for(auto i = 0; i < aoeObjects.size(); i++) {
        auto&& [startTurn, areaOfEffect] = aoeObjects[i];

        areaOfEffect->update(timeSinceLastFrame);

        if(turnController.getTurnNumber() - startTurn >= areaOfEffect->getStats().turns) {
            aoeObjectsForDeletion.push_back(i);
        }
    }
}

std::vector<AreaOfEffect*> AreaOfEffectPool::getAoeEffects(void) {
    std::vector<AreaOfEffect*> aoes;

    for(auto&& [_, aoe] : aoeObjects) {
        aoes.push_back(aoe.get());
    }

    return aoes;
}