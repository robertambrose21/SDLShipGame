#include "inventorypanel.h"

InventoryPanel::InventoryPanel(int width, int height) :
    width(width),
    height(height),
    isShown(false)
{ }

void InventoryPanel::draw(TurnController::Participant* participant) {
    if(!isShown || participant == nullptr) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Inventory");

    for(auto item : participant->items) {
        ImGui::Text("[%s]", item->getName().c_str());
    }

    ImGui::End();
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

// If you don't know the half of it don't let your lips move, focus on yourself you bombaclaat nincompoop