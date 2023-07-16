#pragma once

#include "panel.h"

class PlayerPanel {
private:
    const int PanelHeight = 200;

    std::unique_ptr<Panel> panel;

public:
    PlayerPanel(int width, int height);

    void draw(SDL_Renderer* renderer);
};