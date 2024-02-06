#include "inventorypanel.h"

InventoryPanel::InventoryPanel(int width, int height) :
    width(width),
    height(height),
    isShown(false)
{ }

void InventoryPanel::draw(GraphicsContext& graphicsContext, TurnController::Participant* participant) {
    if(!isShown || participant == nullptr) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Inventory");

    // TODO: This table should only show up if we have an entity selected
    drawEquipment(graphicsContext, participant);
    ImGui::Separator();
    drawWeapons(graphicsContext, participant);

    ImGui::Separator();
    ImGui::BeginChild("Bag");
    
    for(auto item : participant->items) {
        drawInventoryItem(graphicsContext, item);
    }

    ImGui::EndChild();

    ImGui::End();
}

void InventoryPanel::drawEquipment(GraphicsContext& graphicsContext, TurnController::Participant* participant) {
    ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit;

    if(ImGui::BeginTable("EquipmentTable", 2, flags)) {
        for(int i = 0; i < Equipment::Slot::COUNT; i++) {
            auto slot = (Equipment::Slot) i;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", Equipment::SlotToItemType.at(slot).c_str());
            ImGui::TableNextColumn();

            auto equipment = participant->entities[0]->getEquipment(slot);

            if(equipment != nullptr) {
                drawEquippedItem(graphicsContext, equipment->getItem());
            }
            else {
                ImGui::TextColored(ImVec4(.3f, .3f, .3f, 1), "<None>");
            }
        }

        ImGui::EndTable();
    }
}

void InventoryPanel::drawWeapons(GraphicsContext& graphicsContext, TurnController::Participant* participant) {
    ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit;

    if(ImGui::BeginTable("WeaponsTable", 2, flags)) {
        int weaponNumber = 1;
        for(auto weapon : participant->entities[0]->getWeapons()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(1, 1, 1, 1), "Weapon %d", weaponNumber++);
            ImGui::TableNextColumn();

            drawEquippedWeapon(graphicsContext, weapon);
        }

        ImGui::EndTable();
    }
}

void InventoryPanel::drawInventoryItem(GraphicsContext& graphicsContext, Item* item) {
    auto texture = graphicsContext.getTextureLoader().loadTexture(item->getTextureId())->getSDLTexture();
    auto selectableLabel = "##SelectableItem" + std::to_string(item->getId());

    Equipment::Slot slot;
    Equipment::getSlotFromItemType(&slot, item->getType());

    ImGui::SetNextItemAllowOverlap();
    ImGui::Selectable(selectableLabel.c_str());
    if(ImGui::BeginPopupContextItem()) {
        if(item->isWeapon()) {
            if(ImGui::Button("Equip")) {
                onEquipWeaponClicked(item);
            }
        }
        else if(item->isEquippable()) {
            if(ImGui::Button("Equip")) {
                onEquipClicked(item, slot);
            }
        }
        if(ImGui::Button("Examine")) {
            onExamineClicked(item);
        }

        ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::Image((void*) texture, ImVec2(20, 20));
    ImGui::SameLine();
    ImGui::TextColored(ItemRarityColours.at(item->getRarity()), "[%s]", item->getName().c_str());
}

void InventoryPanel::drawEquippedItem(GraphicsContext& graphicsContext, Item* item) {
    auto texture = graphicsContext.getTextureLoader().loadTexture(item->getTextureId())->getSDLTexture();
    auto selectableLabel = "##SelectableEquipment" + std::to_string(item->getId());

    Equipment::Slot slot;
    Equipment::getSlotFromItemType(&slot, item->getType());

    ImGui::SetNextItemAllowOverlap();
    ImGui::Selectable(selectableLabel.c_str());
    if(ImGui::BeginPopupContextItem()) {
        if(ImGui::Button("Unequip")) {
            onUnequipClicked(item, slot);
        }
        if(ImGui::Button("Examine")) {
            onExamineClicked(item);
        }

        ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::Image((void*) texture, ImVec2(20, 20));
    ImGui::SameLine();
    ImGui::TextColored(ItemRarityColours.at(item->getRarity()), "[%s]", item->getName().c_str());
}

void InventoryPanel::drawEquippedWeapon(GraphicsContext& graphicsContext, Weapon* weapon) {
    auto item = weapon->getItem();
    auto texture = graphicsContext.getTextureLoader().loadTexture(item->getTextureId())->getSDLTexture();
    auto selectableLabel = "##SelectableWeapon" + weapon->getId().getString();

    ImGui::SetNextItemAllowOverlap();
    ImGui::Selectable(selectableLabel.c_str());
    if(ImGui::BeginPopupContextItem()) {
        if(ImGui::Button("Unequip")) {
            onUnequipWeaponClicked(weapon);
        }
        if(ImGui::Button("Examine")) {
            onExamineClicked(item);
        }

        ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::Image((void*) texture, ImVec2(20, 20));
    ImGui::SameLine();
    ImGui::TextColored(ItemRarityColours.at(item->getRarity()), "[%s]", item->getName().c_str());
}

void InventoryPanel::show(void) {
    isShown = true;
}

void InventoryPanel::hide(void) {
    isShown = false;
}

void InventoryPanel::toggle(void) {
    isShown = !isShown;
}

void InventoryPanel::addOnEquipCallback(std::function<void(Item* item, Equipment::Slot slot)>&& callback) {
    this->onEquipClicked = callback;
}

void InventoryPanel::addOnUnequipCallback(std::function<void(Item* item, Equipment::Slot slot)>&& callback) {
    this->onUnequipClicked = callback;
}

void InventoryPanel::addOnEquipWeaponClicked(std::function<void(Item* item)>&& callback) {
    this->onEquipWeaponClicked = callback;
}

void InventoryPanel::addOnUnequipWeaponClicked(std::function<void(Weapon* weapon)>&& callback) {
    this->onUnequipWeaponClicked = callback;
}

void InventoryPanel::addOnExamineCallback(std::function<void(Item* item)>&& callback) {
    this->onExamineClicked = callback;
}