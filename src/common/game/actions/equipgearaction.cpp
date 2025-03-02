#include "equipgearaction.h"

EquipGearAction::EquipGearAction(
    Participant* participant,
    Entity* entity, 
    Item* item, 
    Equippable<Stats::GearStats>::Slot slot, 
    bool isUnequip
) :
    Action(participant, entity),
    item(item),
    slot(slot),
    isUnequip(isUnequip)
{ }

EquipGearAction::EquipGearAction(
    Participant* participant, 
    Entity* entity,
    int turnNumber,
    Item* item, 
    Equippable<Stats::GearStats>::Slot slot, 
    bool isUnequip = false
) :
    Action(participant, entity, turnNumber),
    item(item),
    slot(slot),
    isUnequip(isUnequip)
{ }


bool EquipGearAction::onValidate(ApplicationContext* context) {
    if(item == nullptr) {
        spdlog::trace("[EquipItem]: Failed to validate action, item is null");
        return false;
    }

    if(item->getParticipantId() != entity->getParticipantId()) {
        spdlog::trace(
            "[EquipItem]: Failed to validate action, item participant ({}) does not match entity participant ({})",
            item->getParticipantId(),
            entity->getParticipantId()
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
        auto hasEquippedGearSlot = entity->getGear(slot) != nullptr;

        if(!hasEquippedGearSlot) {
            spdlog::trace(
                "[EquipItem]: Failed to validate action, cannot unequip, no item in slot {}",
                Equippable<Stats::GearStats>::SLOT_NAMES[slot]
            );
        }

        return hasEquippedGearSlot;
    }

    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());

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
    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());
    auto existingGear = entity->getGear(slot);

    if(isUnequip) {
        participant->addItem(existingGear->getItem());
        entity->removeGear(slot);
    } else {
        if(existingGear != nullptr) {
            participant->addItem(existingGear->getItem());
        }

        participant->removeItem(item);
        entity->setGear(std::make_unique<Gear>(slot, item));
    }
}

bool EquipGearAction::hasFinished(void) {
    return true;
}

bool EquipGearAction::passesPrecondition(void) {
    return true;
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