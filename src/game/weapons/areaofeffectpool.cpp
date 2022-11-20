#include "areaofeffectpool.h"

AreaOfEffectPool::AreaOfEffectPool(
    std::shared_ptr<TurnController> turnController, 
    std::shared_ptr<Grid> grid
) :
    turnController(turnController),
    grid(grid)
{
    loadAoeDefinitions();
}

void AreaOfEffectPool::loadAoeDefinitions(void) {
    std::string directory = "../assets/data/areaofeffects";

    for(const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::ifstream f(entry.path());
        json data = json::parse(f);

        AoeDefinition definition;
        definition.filename = entry.path();
        definition.name = data["name"].get<std::string>();
        definition.textureId = data["textureId"].get<uint8_t>();
        definition.radius = data["radius"].get<float>();
        definition.turns = data["turns"].get<int>();

        std::cout << "Loaded area of effect definition \"" << definition.name << "\"" << std::endl;

        aoeDefinitions[definition.name] = definition;
    }
}

void AreaOfEffectPool::add(std::shared_ptr<AreaOfEffect> areaOfEffect) {
    aoeObjects.push_back(std::make_pair(turnController->getTurnNumber(), areaOfEffect));
}

void AreaOfEffectPool::add(const std::string& name, int turnNumber, const glm::ivec2& position) {
    if(!aoeDefinitions.contains(name)) {
        throw std::runtime_error("Could not find area of effect definition with name " + name);
    }
    
    auto definition = aoeDefinitions[name];
    auto aoe = std::make_shared<AreaOfEffect>(
        grid,
        definition.textureId,
        turnNumber,
        position,
        AreaOfEffect::Stats { definition.radius, definition.turns }
    );

    add(aoe);
}

void AreaOfEffectPool::draw(std::shared_ptr<GraphicsContext> graphicsContext) {
    for(auto areaOfEffect : aoeObjects) {
        areaOfEffect.second->draw(graphicsContext);
    }
}

void AreaOfEffectPool::update(const uint32_t& timeSinceLastFrame) {
    for(auto aoeObject : aoeObjects) {
        auto [startTurn, areaOfEffect] = aoeObject;

        areaOfEffect->update(timeSinceLastFrame);

        if(turnController->getTurnNumber() - startTurn >= areaOfEffect->getStats().turns) {
            aoeObjectsForDeletion.push_back(aoeObject);
        }
    }

     for(auto areaOfEffect : aoeObjectsForDeletion) {
        aoeObjects.erase(std::find(aoeObjects.begin(), aoeObjects.end(), areaOfEffect));
    }
    
    aoeObjectsForDeletion.clear();
}