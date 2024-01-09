#include "takeitemaction.h"

TakeItemAction::TakeItemAction(int turnNumber, Entity* entity, const std::vector<Item*>& items) :
    Action(turnNumber, entity),
    items(items)
{ }

bool TakeItemAction::onValidate(void) {
    if(items.empty()) {
        return false;
    }

    for(auto action : entity->getActionsChain(turnNumber)) {
        if(
            action->getType() == Action::Type::TakeItem && 
            containsAny(items, dynamic_cast<TakeItemAction*>(action)->getItems())
        ) {
            return false;
        }
    }

    return true;
}

void TakeItemAction::onExecute(ApplicationContext* context) {
    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());

    if(!participant->isPlayer) {
        return;
    }

    std::vector<uint32_t> itemIds;
    for(auto item : items) {
        itemIds.push_back(item->getId());
        participant->items.push_back(item->getName());
    }

    context->getItemController()->removeItems(itemIds);
}

bool TakeItemAction::hasFinished(void) {
    return true;
}

bool TakeItemAction::containsAny(std::vector<Item*> itemsA, std::vector<Item*> itemsB) {
    if(itemsA.empty() || itemsB.empty()) {
        return false;
    }

    for(auto itemA : itemsA) {
        for(auto itemB : itemsB) {
            if(itemA->getId() == itemB->getId()) {
                return true;
            }
        }
    }

    return false;
}

bool TakeItemAction::passesPrecondition(void) {
    return true;
}

Action::Type TakeItemAction::getType(void) {
    return Action::Type::TakeItem;
}

const std::vector<Item*>& TakeItemAction::getItems(void) const {
    return items;
}