#include "examineitempanel.h"

ExamineItemPanel::ExamineItemPanel(Item* item) :
    item(item),
    isOpen(true)
{ }

void ExamineItemPanel::draw(GraphicsContext& graphicsContext) {
    if(!isOpen) {
        return;
    }

    auto texture = graphicsContext.getTextureLoader().loadTexture(item->getTextureId())->getSDLTexture();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(300, 200));
    ImGui::Begin(item->getName().c_str(), &isOpen, flags);

    ImGui::Image((void*) texture, ImVec2(30, 30));

    ImGui::SameLine();
    ImGui::AlignTextToFramePadding();
    ImGui::TextColored(ItemRarityColours.at(item->getRarity()), "[%s]", item->getName().c_str());
    ImGui::Separator();
    ImGui::Text("%s", item->getType().c_str());

    ImGui::Separator();

    ImGui::BeginChild("Stats");
    for(auto& [name, value] : item->getStats().getValues()) {
        ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "%s: ", name.c_str());
        ImGui::SameLine(0, 0);
        ImGui::Text("%s", value.c_str());
    }
    ImGui::EndChild();

    ImGui::End();
}

bool ExamineItemPanel::getIsOpen(void) const {
    return isOpen;
}
