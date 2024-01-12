#pragma once

#include <string>

#include "text.h"
#include "panel.h"

class TextPanel : public Panel {
private:
    std::string lines;
    std::unique_ptr<Text> text;

public:
    TextPanel(std::unique_ptr<Font> font, const glm::ivec2& position, const glm::ivec2& size);

    void draw(SDL_Renderer* renderer);
    void writeLine(const std::string& line);
    void setText(const std::string& text);
};