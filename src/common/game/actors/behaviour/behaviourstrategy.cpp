#include "behaviourstrategy.h"

BehaviourStrategy::BehaviourStrategy(ApplicationContext& context) :
    context(context)
{ }

ApplicationContext& BehaviourStrategy::getContext(void) {
    return context;
}