#include "takeitemaction.h"

TakeItemAction::TakeItemAction(Participant* participant, Entity* entity, const std::vector<Item*>& items) :
    Action(participant, entity),
    items(items)
{ }

TakeItemAction::TakeItemAction(
    Participant* participant, 
    Entity* entity,
    int turnNumber,
    const std::vector<Item*>& items
) :
    Action(participant, entity, turnNumber),
    items(items)
{ }


bool TakeItemAction::onValidate(ApplicationContext* context) {
    if(items.empty()) {
        spdlog::trace("[TakeItem]: Failed to validate action, no items to take");
        return false;
    }

    if(participant->getEngagement() == nullptr || !turnNumber.has_value()) {
        return true;
    }

    for(auto action : entity->getActionsChain(turnNumber.value())) {
        if(
            action->getType() == Action::Type::TakeItem && 
            containsAny(items, dynamic_cast<TakeItemAction*>(action)->getItems())
        ) {
            spdlog::trace(
                "[{}, TakeItem]: Failed to validate action, there are already actions on the chain to take these items [{}]",
                turnNumber,
                getItemsAsStringList()
            );
            return false;
        }
    }

    return true;
}

void TakeItemAction::onExecute(ApplicationContext* context) {
    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());

    if(!participant->getIsPlayer()) {
        return;
    }

    for(auto item : items) {
        item->setParticipantId(participant->getId());
        participant->addItem(item);
    }
    
    context->getItemController()->flagWorldItemsDirty();
}

bool TakeItemAction::hasFinished(void) {
    return true;
}

bool TakeItemAction::containsAny(const std::vector<Item*>& itemsA, const std::vector<Item*>& itemsB) {
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

std::string TakeItemAction::getItemsAsStringList(void) {
    std::string itemList = "";

    for(size_t i = 0; i < items.size(); ++i) {
        itemList += std::format("Item[{}#{}]", items[i]->getName(), items[i]->getId());
        if(i < items.size() - 1) {
            itemList += ", ";
        }
    }

    return itemList;
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