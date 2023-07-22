#include "textpanel.h"

TextPanel::TextPanel(std::unique_ptr<Font> font, const glm::ivec2& position, const glm::ivec2& size) :
    Panel(position, size)
{
    text = std::make_unique<Text>(std::move(font), position, size); // TODO: Padding
}

void TextPanel::draw(SDL_Renderer* renderer) {
    Panel::draw(renderer);
    text->draw(renderer, lines);
}

void TextPanel::writeLine(const std::string& line) {
    lines.append(line + "\n");
}