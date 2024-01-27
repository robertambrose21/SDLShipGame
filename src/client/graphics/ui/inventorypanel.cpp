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

    for(auto item : participant->items) {
        auto texture = graphicsContext.getTextureLoader().loadTexture(item->getTextureId())->getSDLTexture();

        ImGui::Image((void*) texture, ImVec2(20, 20));
        ImGui::SameLine();
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