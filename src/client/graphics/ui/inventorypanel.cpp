#include "inventorypanel.h"

InventoryPanel::InventoryPanel(int width, int height) :
    width(width),
    height(height),
    isShown(false)
{ }

void InventoryPanel::draw(GraphicsContext& graphicsContext, Participant* participant) {
    if(!isShown || participant == nullptr) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Inventory");

    ImGui::BeginChild("Bag");
    
    for(auto item : participant->getItems()) {
        drawInventoryItem(graphicsContext, item);
    }

    ImGui::EndChild();

    ImGui::End();
}


void InventoryPanel::drawInventoryItem(GraphicsContext& graphicsContext, Item* item) {
    auto texture = graphicsContext.getTextureLoader().loadTexture(item->getTextureId())->getSDLTexture();
    auto selectableLabel = "##SelectableItem" + std::to_string(item->getId());

    Equippable<Stats::GearStats>::Slot slot = Equippable<Stats::GearStats>::getSlotFromName(item->getType());

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

void InventoryPanel::show(void) {
    isShown = true;
}

void InventoryPanel::hide(void) {
    isShown = false;
}

void InventoryPanel::toggle(void) {
    isShown = !isShown;
}

void InventoryPanel::addOnEquipCallback(std::function<void(Item* item, Equippable<Stats::GearStats>::Slot slot)>&& callback) {
    this->onEquipClicked = callback;
}

void InventoryPanel::addOnEquipWeaponCallback(std::function<void(Item* item)>&& callback) {
    this->onEquipWeaponClicked = callback;
}

void InventoryPanel::addOnExamineCallback(std::function<void(Item* item)>&& callback) {
    this->onExamineClicked = callback;
}