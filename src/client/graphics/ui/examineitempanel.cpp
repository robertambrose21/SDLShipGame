#include "examineitempanel.h"

ExamineItemPanel::ExamineItemPanel(Item* item) :
    item(item),
    isOpen(true)
{
    stats = buildStats();
}

std::map<StatsValue::Type, std::vector<ExamineItemPanel::StatsPair>> ExamineItemPanel::buildStats(void) {
    std::map<StatsValue::Type, std::vector<StatsPair>> stats;

    for(auto& [name, value] : item->getStats().getValues()) {
        stats[value.type].push_back({ name, value.value });
    }

    return stats;
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
    drawStatsTable(StatsValue::Weapon);
    drawStatsTable(StatsValue::Projectile);
    drawStatsTable(StatsValue::AreaOfEffect);
    ImGui::EndChild();

    ImGui::End();
}

void ExamineItemPanel::drawStatsTable(StatsValue::Type type) {
    if(!stats.contains(type)) {
        return;
    }

    auto typeString = getStatsTypeString(type).c_str();

    ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit;

    ImGui::Text("%s", typeString);
    ImGui::Separator();

    if(ImGui::BeginTable(typeString, 2, flags)) {
        for(auto& [name, value] : stats[type]) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "%s: ", name.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", value.c_str());
        }

        ImGui::EndTable();
    }
}

std::string ExamineItemPanel::getStatsTypeString(StatsValue::Type type) {
    switch(type) {
        case StatsValue::Common:
            return "Common";
        case StatsValue::Effect:
            return "Effect";
        case StatsValue::AreaOfEffect:
            return "Area of Effect";
        case StatsValue::Projectile:
            return "Projectile";
        case StatsValue::Weapon:
            return "Weapon";
        case StatsValue::All:
            return "All";
        default:
            return "Unknown";
    }
}

bool ExamineItemPanel::getIsOpen(void) const {
    return isOpen;
}
