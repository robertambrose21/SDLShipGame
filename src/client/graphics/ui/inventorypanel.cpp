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
    ImGui::BeginChild("Bag");
    
    for(auto item : participant->items) {
        drawItem(graphicsContext, item, false);
    }

    ImGui::EndChild();

    ImGui::End();
}

void InventoryPanel::drawEquipment(GraphicsContext& graphicsContext, TurnController::Participant* participant) {
    if(ImGui::BeginTable("EquipmentTable", 2)) {
        for(int i = 0; i < Equipment::Slot::COUNT; i++) {
            auto slot = (Equipment::Slot) i;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", Equipment::SlotToItemType.at(slot).c_str());
            ImGui::TableNextColumn();

            auto equipment = participant->entities[0]->getEquipment(slot);

            if(equipment != nullptr) {
                drawItem(graphicsContext, equipment->getItem(), true);
            }
            else {
                ImGui::TextColored(ImVec4(.3f, .3f, .3f, 1), "<None>");
            }
        }

        ImGui::EndTable();
    }
}

void InventoryPanel::drawItem(GraphicsContext& graphicsContext, Item* item, bool isEquipped) {
    auto texture = graphicsContext.getTextureLoader().loadTexture(item->getTextureId())->getSDLTexture();
    auto selectableLabel = "##SelectableItem" + std::to_string(item->getId());

    Equipment::Slot slot;
    Equipment::getSlotFromItemType(&slot, item->getType());

    ImGui::SetNextItemAllowOverlap();
    ImGui::Selectable(selectableLabel.c_str());
    if(ImGui::BeginPopupContextItem()) {
        if(isEquipped) {
            if(ImGui::Button("Unequip")) {
                onUnequipClicked(item, slot);
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

void InventoryPanel::addOnExamineCallback(std::function<void(Item* item)>&& callback) {
    this->onExamineClicked = callback;
}