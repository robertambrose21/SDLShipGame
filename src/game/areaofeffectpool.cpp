#include "areaofeffectpool.h"

AreaOfEffectPool::AreaOfEffectPool()
{ }

void AreaOfEffectPool::add(std::shared_ptr<AreaOfEffect> areaOfEffect) {
    aoeObjects.push_back(areaOfEffect);
}

void AreaOfEffectPool::draw(std::shared_ptr<SDL_Renderer> renderer) {
    for(auto areaOfEffect : aoeObjects) {
        areaOfEffect->draw(renderer);
    }
}

void AreaOfEffectPool::update(const Uint32& timeSinceLastFrame) {
    for(auto areaOfEffect : aoeObjects) {
        areaOfEffect->update(timeSinceLastFrame);
    }

     for(auto areaOfEffect : aoeObjectsForDeletion) {
        aoeObjects.erase(std::find(aoeObjects.begin(), aoeObjects.end(), areaOfEffect));
    }
    
    aoeObjectsForDeletion.clear();
}