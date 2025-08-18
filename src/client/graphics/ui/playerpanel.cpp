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

void PlayerPanel::onPublish(const Event<ActorEventData>& event) {
    if(event.data.type == "Death") {
        lines.push_back({
            { getTimestampString(event.timestamp), TimestampColour },
            { getActorIdentifier(event.data.actor), HighlightColour },
            { " died", StdTextColour}
        });
    }
    else if(event.data.type == "Freeze" && event.data.actor->getIsFrozen()) {
        lines.push_back({
            { getTimestampString(event.timestamp), TimestampColour },
            { getActorIdentifier(event.data.actor), HighlightColour },
            { " unfreezes", StdTextColour }
        });
    }
}

void PlayerPanel::onPublish(const Event<MeleeWeaponEventData>& event) {
    if(event.data.weapon->getType() != Stats::WeaponStats::MELEE) {
        return;
    }

    lines.push_back({
        { getTimestampString(event.timestamp), TimestampColour },
        { getActorIdentifier(event.data.owner), HighlightColour },
        { " meleed ", StdTextColour },
        { getActorIdentifier(event.data.target), HighlightColour },
        { " for ", StdTextColour },
        { std::to_string(event.data.damage), HighlightColour },
        { " damage! ", StdTextColour },
        { getActorIdentifier(event.data.target), HighlightColour },
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
            { getActorIdentifier(event.data.target), HighlightColour },
            { " was hit by a projectile from participant [", StdTextColour },
            { std::to_string(event.data.projectile->getOwnerId()), HighlightColour },
            { "] and took ", StdTextColour },
            { std::to_string(event.data.damage), HighlightColour },
            { " damage! ", StdTextColour },
            { getActorIdentifier(event.data.target), HighlightColour },
            { " now has ", StdTextColour },
            { std::to_string(event.data.target->getCurrentHP()), HighlightColour },
            { " HP ", StdTextColour }
        });
    }

    auto effects = event.data.projectile->getStats().effects;
    for(auto effect : effects) {
        if(effect.type == EffectType::FREEZE) {
            lines.push_back({
                { getTimestampString(event.timestamp), TimestampColour },
                { getActorIdentifier(event.data.target), HighlightColour },
                { " is frozen for ", StdTextColour },
                { std::to_string(effect.duration), HighlightColour },
                { " turns", StdTextColour }
            });
        }
        else if(effect.type == EffectType::POISON) {
            lines.push_back({
                { getTimestampString(event.timestamp), TimestampColour },
                { getActorIdentifier(event.data.target), HighlightColour },
                { " is poisoned for ", StdTextColour },
                { std::to_string(effect.duration), HighlightColour },
                { " turns", StdTextColour }
            });
        }
    }
}

void PlayerPanel::onPublish(const Event<AreaOfEffectEventData>& event) {
    lines.push_back({
        { getTimestampString(event.timestamp), TimestampColour },
        { getActorIdentifier(event.data.target), HighlightColour },
        { " was hit by an area of effect from participant [", StdTextColour },
        { std::to_string(event.data.aoe->getOwnerId()), HighlightColour },
        { "] and took ", StdTextColour },
        { std::to_string(event.data.damage), HighlightColour },
        { " damage! ", StdTextColour },
        { getActorIdentifier(event.data.target), HighlightColour },
        { " now has ", StdTextColour },
        { std::to_string(event.data.target->getCurrentHP()), HighlightColour },
        { " HP ", StdTextColour }
    });
}

void PlayerPanel::onPublish(const Event<ItemEventData>& event) {
    if(event.data.type == ItemEventData::Type::REMOVED) {
        return;
    }
    std::vector<TextSegment> line;

    if(event.data.droppedBy != Item::UnknownOwner) {
        line = {
            { getTimestampString(event.timestamp), TimestampColour },
            { event.data.droppedBy, HighlightColour },
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
        { getActorIdentifier(event.data.actor), HighlightColour },
        { " picked up items: ", StdTextColour }
    };

    appendItemsToLine(line, event.data.items);

    lines.push_back(line);
}

void PlayerPanel::onPublish(const Event<ApplyDamageEventData>& event) {
    std::vector<TextSegment> line = {
        { getTimestampString(event.timestamp), TimestampColour },
        { getActorIdentifier(event.data.target), HighlightColour }
    };

    switch(event.data.source) {
        case DamageType::AOE:
            line.push_back({ " was hit by an area of effect from participant [", StdTextColour });
            break;

        case DamageType::PROJECTILE:
            line.push_back({ " was hit by a projectile from participant [", StdTextColour });
            break;

        case DamageType::MELEE:
            line.push_back({ " was meleed by participant [", StdTextColour });
            break;

        default:
            break;
    }

    line.push_back({ std::to_string(event.data.participantId), HighlightColour });
    line.push_back({ "] and took ", StdTextColour });
    line.push_back({ std::to_string(event.data.damage), HighlightColour });
    line.push_back({ " damage! ", StdTextColour });
    line.push_back({ getActorIdentifier(event.data.target), HighlightColour });
    line.push_back({ " now has ", StdTextColour });
    line.push_back({ std::to_string(event.data.target->getCurrentHP()), StdTextColour });
    line.push_back({ " HP.", StdTextColour });

    lines.push_back(line);
}

std::string PlayerPanel::getActorIdentifier(Actor* actor) {
    game_assert(actor != nullptr);
    return actor->getName() + "#" + std::to_string(actor->getId());
}

std::string PlayerPanel::getTimestampString(std::time_t timestamp) {
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&timestamp), "[%H:%M:%S]: ");
    return oss.str();
}

void PlayerPanel::appendItemsToLine(std::vector<TextSegment>& segment, const std::vector<Item*>& items) {
    for(int i = 0; i < items.size(); i++) {
        segment.push_back({ "[" + items[i]->getName() + "]" , ItemRarityColours.at(items[i]->getRarity()) });

        if(i < items.size() - 1) {
            segment.push_back({ ", ", StdTextColour });
        }
    }
}