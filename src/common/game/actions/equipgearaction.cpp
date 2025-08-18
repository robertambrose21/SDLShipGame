#include "equipgearaction.h"
#include "game/participant/participant.h"

EquipGearAction::EquipGearAction(
    Participant* participant,
    Actor* actor, 
    Item* item, 
    Equippable<Stats::GearStats>::Slot slot, 
    bool isUnequip
) :
    Action(participant, actor),
    item(item),
    slot(slot),
    isUnequip(isUnequip)
{ }

EquipGearAction::EquipGearAction(
    Participant* participant, 
    Actor* actor,
    int turnNumber,
    Item* item, 
    Equippable<Stats::GearStats>::Slot slot, 
    bool isUnequip
) :
    Action(participant, actor, turnNumber),
    item(item),
    slot(slot),
    isUnequip(isUnequip)
{ }

ActionVariant EquipGearAction::getPublishData(void) {
    return EquipItemActionEventData { turnNumber, actor, item, slot, isUnequip };
}

bool EquipGearAction::onValidate(ApplicationContext* context) {
    if(item == nullptr) {
        spdlog::trace("[EquipItem]: Failed to validate action, item is null");
        return false;
    }

    if(item->getParticipantId() != actor->getParticipantId()) {
        spdlog::trace(
            "[EquipItem]: Failed to validate action, item participant ({}) does not match actor participant ({})",
            item->getParticipantId(),
            actor->getParticipantId()
        );
        return false;
    }

    if(!contains(Gear::VALID_SLOTS, slot)) {
        spdlog::trace(
            "[EquipItem]: Failed to validate action, cannot equip invalid slot {}",
            Equippable<Stats::GearStats>::SLOT_NAMES[slot]
        );
        return false;
    }

    if(isUnequip) {
        auto hasEquippedGearSlot = actor->getGear(slot) != nullptr;

        if(!hasEquippedGearSlot) {
            spdlog::trace(
                "[EquipItem]: Failed to validate action, cannot unequip, no item in slot {}",
                Equippable<Stats::GearStats>::SLOT_NAMES[slot]
            );
        }

        return hasEquippedGearSlot;
    }

    auto participant = context->getGameController()->getParticipant(actor->getParticipantId());

    bool hasItem = false;   

    for(auto itemInInventory : participant->getItems()) {
        if(item->getId() == itemInInventory->getId()) {
            hasItem = true;
        }
    }

    if(!hasItem) {
        spdlog::trace(
            "[EquipItem]: Failed to validate action, cannot find Item[{}] in inventory for participant {}",
            item->getId(),
            participant->getId()
        );
    }

    return hasItem;
}

void EquipGearAction::onExecute(ApplicationContext* context) {
    auto participant = context->getGameController()->getParticipant(actor->getParticipantId());
    auto existingGear = actor->getGear(slot);

    if(isUnequip) {
        participant->addItem(existingGear->getItem());
        actor->removeGear(slot);
    } else {
        if(existingGear != nullptr) {
            participant->addItem(existingGear->getItem());
        }

        participant->removeItem(item);
        actor->setGear(std::make_unique<Gear>(slot, item));
    }
}

bool EquipGearAction::hasFinished(void) {
    return true;
}

bool EquipGearAction::passesPrecondition(void) {
    return !participant->hasAnyEngagement();
}

Action::Type EquipGearAction::getType(void) {
    return Action::Type::EquipItem;
}

Item* EquipGearAction::getItem(void) {
    return item;
}

Equippable<Stats::GearStats>::Slot EquipGearAction::getSlot(void) const {
    return slot;
}

bool EquipGearAction::getIsUnequip(void) const {
    return isUnequip;
}