#pragma once

#include "panel.h"

class PlayerPanel {
private:
    const int PanelHeight = 200;

    std::shared_ptr<Panel> panel;

public:
    PlayerPanel(int width, int height);

    void draw(const std::shared_ptr<SDL_Renderer>& renderer);
};