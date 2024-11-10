#include "examineitempanel.h"

ExamineItemPanel::ExamineItemPanel(Item* item) :
    item(item),
    isOpen(true)
{
    stats = Stats::calculateItemStatCategories(item->getStats());
}

void ExamineItemPanel::draw(GraphicsContext& graphicsContext) {
    if(!isOpen) {
        return;
    }

    auto texture = graphicsContext.getTextureLoader().loadTexture(item->getTextureId())->getSDLTexture();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(300, 500));
    ImGui::Begin(item->getName().c_str(), &isOpen, flags);

    ImGui::Image((void*) texture, ImVec2(30, 30));

    ImGui::SameLine();
    ImGui::AlignTextToFramePadding();
    ImGui::TextColored(ItemRarityColours.at(item->getRarity()), "[%s]", item->getName().c_str());
    ImGui::Separator();

    ImGui::BeginChild("Stats");
    drawStatsTable();
    ImGui::EndChild();

    ImGui::End();
}

void ExamineItemPanel::drawStatsTable(void) {
    ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit;

    for(auto const& [category, pairs] : stats) {
        auto categoryString = Stats::statCategoryToString(category).c_str();

        ImGui::Text("%s", categoryString);
        ImGui::Separator();

        if(ImGui::BeginTable(categoryString, 2, flags)) {
            for(auto const [name, value] : pairs) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "%s: ", name.c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", value.c_str());
            }
        

            ImGui::EndTable();
        }
    }
}

bool ExamineItemPanel::getIsOpen(void) const {
    return isOpen;
}
