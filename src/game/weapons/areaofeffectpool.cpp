#include "areaofeffectpool.h"

AreaOfEffectPool::AreaOfEffectPool(std::shared_ptr<TurnController> turnController) :
    turnController(turnController)
{ }

void AreaOfEffectPool::add(std::shared_ptr<AreaOfEffect> areaOfEffect) {
    aoeObjects.push_back(std::make_pair(turnController->getTurnNumber(), areaOfEffect));
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