#include "playerpanel.h"

PlayerPanel::PlayerPanel(int width, int height) :
    width(width),
    height(height)
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("../assets/fonts/RobotoMono-SemiBold.ttf", 20.0f);
}

void PlayerPanel::draw(void) {
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize;

    ImGui::SetNextWindowPos(ImVec2(0, height - PanelHeight));
    ImGui::SetNextWindowSize(ImVec2(width, PanelHeight));
    ImGui::Begin("Panel", NULL, flags);

    for(auto line : lines) {
        for(auto [text, colour] : line) {
            ImGui::TextColored(colour, "%s", text.c_str());
            ImGui::SameLine(0, 0);
        }
        ImGui::NewLine();
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::End();
}

void PlayerPanel::onPublish(const Event<TurnEventData>& event) {
    lines.push_back({
        { getTimestampString(event.timestamp), TimestampColour },
        { std::format("Turn {}", event.data.turnNumber), StdTextColour }
    });
}

void PlayerPanel::onPublish(const Event<EntityEventData>& event) {
    if(event.data.type == "Death") {
        lines.push_back({
            { getTimestampString(event.timestamp), TimestampColour },
            { getEntityIdentifier(event.data.entity), HighlightColour },
            { " died", StdTextColour}
        });
    }
    else if(event.data.type == "Freeze" && event.data.entity->getFrozenFor() <= 0) {
        lines.push_back({
            { getTimestampString(event.timestamp), TimestampColour },
            { getEntityIdentifier(event.data.entity), HighlightColour },
            { " unfreezes", StdTextColour }
        });
    }
}

void PlayerPanel::onPublish(const Event<WeaponEventData>& event) {
    if(event.data.weapon->getType() != Weapon::Type::MELEE) {
        return;
    }

    lines.push_back({
        { getTimestampString(event.timestamp), TimestampColour },
        { getEntityIdentifier(event.data.owner), HighlightColour },
        { " meleed ", StdTextColour },
        { getEntityIdentifier(event.data.target), HighlightColour },
        { " for ", StdTextColour },
        { std::to_string(event.data.weapon->getStats().damage), HighlightColour },
        { " damage! ", StdTextColour },
        { getEntityIdentifier(event.data.target), HighlightColour },
        { " now has ", StdTextColour },
        { std::to_string(event.data.target->getCurrentHP()), HighlightColour },
        { " HP ", StdTextColour }
    });
}

void PlayerPanel::onPublish(const Event<ProjectileEventData>& event) {
    if(event.data.target == nullptr) {
        return;
    }

    if(event.data.damage > 0) {      
        lines.push_back({
            { getTimestampString(event.timestamp), TimestampColour },
            { getEntityIdentifier(event.data.target), HighlightColour },
            { " was hit by a projectile from participant [", StdTextColour },
            { std::to_string(event.data.projectile->getOwnerId()), HighlightColour },
            { "] and took ", StdTextColour },
            { std::to_string(event.data.damage), HighlightColour },
            { " damage! ", StdTextColour },
            { getEntityIdentifier(event.data.target), HighlightColour },
            { " now has ", StdTextColour },
            { std::to_string(event.data.target->getCurrentHP()), HighlightColour },
            { " HP ", StdTextColour }
        });
    }

    auto effects = event.data.projectile->getStats().effects;
    for(auto effect : effects) {
        if(effect.name == "freeze") {
            lines.push_back({
                { getTimestampString(event.timestamp), TimestampColour },
                { getEntityIdentifier(event.data.target), HighlightColour },
                { " is frozen for ", StdTextColour },
                { std::to_string(effect.duration), HighlightColour },
                { " turns", StdTextColour }
            });
        }
    }
}

void PlayerPanel::onPublish(const Event<AreaOfEffectEventData>& event) {
    lines.push_back({
        { getTimestampString(event.timestamp), TimestampColour },
        { getEntityIdentifier(event.data.target), HighlightColour },
        { " was hit by an area of effect from participant [", StdTextColour },
        { std::to_string(event.data.aoe->getOwnerId()), HighlightColour },
        { "] and took ", StdTextColour },
        { std::to_string(event.data.aoe->getStats().damagePerTurn), HighlightColour },
        { " damage! ", StdTextColour },
        { getEntityIdentifier(event.data.target), HighlightColour },
        { " now has ", StdTextColour },
        { std::to_string(event.data.target->getCurrentHP()), HighlightColour },
        { " HP ", StdTextColour }
    });
}

void PlayerPanel::onPublish(const Event<ItemEventData>& event) {
    if(event.data.type == ItemEventData::Type::REMOVED) {
        return;
    }

    if(event.data.owner == nullptr) {
        return;
    }

    std::vector<TextSegment> line;

    if(event.data.owner != nullptr) {
        line = {
            { getTimestampString(event.timestamp), TimestampColour },
            { getEntityIdentifier(event.data.owner), HighlightColour },
            { " dropped ", StdTextColour }
        };
        appendItemsToLine(line, event.data.items);
    }
    else {
        line = {
            { getTimestampString(event.timestamp), TimestampColour }
        };
        appendItemsToLine(line, event.data.items);
        line.push_back({ " was dropped", StdTextColour });
    }

    lines.push_back(line);
}

void PlayerPanel::onPublish(const Event<TakeItemActionEventData>& event) {
    std::string items = "";

    std::vector<TextSegment> line = {
        { getTimestampString(event.timestamp), TimestampColour },
        { getEntityIdentifier(event.data.entity), HighlightColour },
        { " picked up items: ", StdTextColour }
    };

    appendItemsToLine(line, event.data.items);

    lines.push_back(line);
}

std::string PlayerPanel::getEntityIdentifier(Entity* entity) {
    game_assert(entity != nullptr);
    return entity->getName() + "#" + std::to_string(entity->getId());
}

std::string PlayerPanel::getTimestampString(std::time_t timestamp) {
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&timestamp), "[%H:%M:%S]: ");
    return oss.str();
}

void PlayerPanel::appendItemsToLine(std::vector<TextSegment>& segment, const std::vector<Item*>& items) {
    for(int i = 0; i < items.size(); i++) {
        segment.push_back({ "[" + items[i]->getName() + "]" , HighlightColour });

        if(i < items.size() - 1) {
            segment.push_back({ ", ", StdTextColour });
        }
    }
}