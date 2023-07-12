#include "playerpanel.h"

PlayerPanel::PlayerPanel(int width, int height) {
    panel = std::make_shared<Panel>(glm::ivec2(0, height - PanelHeight), glm::ivec2(width, PanelHeight));
    panel->setColour(0x64, 0x64, 0x64, 0xFF);
}

void PlayerPanel::draw(const std::shared_ptr<SDL_Renderer>& renderer) {
    panel->draw(renderer);
}