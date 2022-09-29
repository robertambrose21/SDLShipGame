#include "areaofeffectpool.h"

AreaOfEffectPool::AreaOfEffectPool(std::shared_ptr<EntityPool> entityPool) :
    entityPool(entityPool)
{ }

void AreaOfEffectPool::add(std::shared_ptr<AreaOfEffect> areaOfEffect) {
    aoeObjects.push_back(std::make_pair(entityPool->getTurnNumber(), areaOfEffect));
}

void AreaOfEffectPool::draw(std::shared_ptr<SDL_Renderer> renderer) {
    for(auto areaOfEffect : aoeObjects) {
        areaOfEffect.second->draw(renderer);
    }
}

void AreaOfEffectPool::update(const Uint32& timeSinceLastFrame) {
    for(auto aoeObject : aoeObjects) {
        auto [startTurn, areaOfEffect] = aoeObject;

        areaOfEffect->update(timeSinceLastFrame);

        if(entityPool->getTurnNumber() - startTurn >= areaOfEffect->getStats().turns) {
            aoeObjectsForDeletion.push_back(aoeObject);
        }
    }

     for(auto areaOfEffect : aoeObjectsForDeletion) {
        aoeObjects.erase(std::find(aoeObjects.begin(), aoeObjects.end(), areaOfEffect));
    }
    
    aoeObjectsForDeletion.clear();
}