#include "areaofeffectpool.h"

AreaOfEffectPool::AreaOfEffectPool(
    const std::shared_ptr<TurnController>& turnController, 
    const std::shared_ptr<Grid>& grid
) :
    turnController(turnController),
    grid(grid)
{
    loadAoeDefinitions();

    turnController->addOnNextTurnFunction([&](auto const& currentParticipant, auto const& turnNumber) {
        for(auto [_, aoe]  : aoeObjects) {
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

void AreaOfEffectPool::add(const std::shared_ptr<AreaOfEffect>& areaOfEffect) {
    aoeObjects.push_back(std::make_pair(turnController->getTurnNumber(), areaOfEffect));
}

void AreaOfEffectPool::add(const std::string& name, int ownerId, int turnNumber, const glm::ivec2& position) {
    game_assert(aoeDefinitions.contains(name));
    auto const& definition = aoeDefinitions[name];
    auto aoe = std::make_shared<AreaOfEffect>(
        grid,
        definition.textureId,
        ownerId,
        turnNumber,
        position,
        AreaOfEffect::Stats { definition.radius, definition.turns, definition.damagePerTurn }
    );

    add(aoe);
}

void AreaOfEffectPool::draw(const std::shared_ptr<GraphicsContext>& graphicsContext) {
    for(auto [_, aoe] : aoeObjects) {
        aoe->draw(graphicsContext);
    }
}

void AreaOfEffectPool::update(uint32_t timeSinceLastFrame) {
    for(auto const& aoeObject : aoeObjects) {
        auto [startTurn, areaOfEffect] = aoeObject;

        areaOfEffect->update(timeSinceLastFrame);

        if(turnController->getTurnNumber() - startTurn >= areaOfEffect->getStats().turns) {
            aoeObjectsForDeletion.push_back(aoeObject);
        }
    }

     for(auto const& areaOfEffect : aoeObjectsForDeletion) {
        aoeObjects.erase(std::find(aoeObjects.begin(), aoeObjects.end(), areaOfEffect));
    }
    
    aoeObjectsForDeletion.clear();
}