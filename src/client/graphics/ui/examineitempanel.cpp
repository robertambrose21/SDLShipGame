#include "examineitempanel.h"

ExamineItemPanel::ExamineItemPanel(Item* item) :
    item(item),
    isOpen(true)
{
    stats = buildStats();
}

std::map<StatsKey::Type, std::vector<ExamineItemPanel::StatsPair>> ExamineItemPanel::buildStats(void) {
    std::map<StatsKey::Type, std::vector<StatsPair>> stats;

    for(auto& [key, value] : item->getStats().getValues()) {
        stats[key.type].push_back({ key.keyName, value });
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
    drawStatsTable(StatsKey::Common);
    drawStatsTable(StatsKey::Effect);
    drawStatsTable(StatsKey::Weapon);
    drawStatsTable(StatsKey::Projectile);
    drawStatsTable(StatsKey::AreaOfEffect);
    ImGui::EndChild();

    ImGui::End();
}

void ExamineItemPanel::drawStatsTable(StatsKey::Type type) {
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

std::string ExamineItemPanel::getStatsTypeString(StatsKey::Type type) {
    switch(type) {
        case StatsKey::Common:
            return "Common";
        case StatsKey::Effect:
            return "Effect";
        case StatsKey::AreaOfEffect:
            return "Area of Effect";
        case StatsKey::Projectile:
            return "Projectile";
        case StatsKey::Weapon:
            return "Weapon";
        case StatsKey::All:
            return "All";
        default:
            return "Unknown";
    }
}

bool ExamineItemPanel::getIsOpen(void) const {
    return isOpen;
}
