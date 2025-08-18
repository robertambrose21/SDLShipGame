#include "actorpanel.h"

ActorPanel::ActorPanel(int width, int height, Actor* actor) :
    width(width),
    height(height),
    actor(actor),
    isOpen(true)
{
    stats = Stats::calculateActorStatCategories(actor->getStats());
}

void ActorPanel::draw(GraphicsContext& graphicsContext) {
    if(!isOpen || actor == nullptr) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Actor", &isOpen);

    drawStats(graphicsContext);
    ImGui::Separator();
    drawEquipment(graphicsContext);
    ImGui::Separator();
    drawWeapons(graphicsContext);

    ImGui::End();
}

void ActorPanel::drawStats(GraphicsContext& graphicsContext) {
    ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit;

    if(ImGui::BeginTable("StatsTable", 2, flags)) {
        for(auto& [_, pairs] : stats) {
            for(auto& stat : pairs) {
                drawStat(stat.name, stat.value);
            }
        }

        ImGui::EndTable();
    }
}

void ActorPanel::drawStat(const std::string& name, const std::string& value) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", name.c_str());
    ImGui::TableNextColumn();
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", value.c_str());
}

void ActorPanel::drawEquipment(GraphicsContext& graphicsContext) {
    ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit;

    if(ImGui::BeginTable("EquipmentTable", 2, flags)) {
        for(auto slot : Gear::VALID_SLOTS) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", Equippable<Stats::GearStats>::SLOT_NAMES[slot].c_str());
            ImGui::TableNextColumn();

            auto gear = actor->getGear(slot);

            if(gear != nullptr) {
                drawEquippedItem(graphicsContext, gear->getItem());
            }
            else {
                ImGui::TextColored(ImVec4(.3f, .3f, .3f, 1), "<None>");
            }
        }

        ImGui::EndTable();
    }
}

void ActorPanel::drawWeapons(GraphicsContext& graphicsContext) {
    ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit;

    if(ImGui::BeginTable("WeaponsTable", 2, flags)) {
        int weaponNumber = 1;
        for(auto weapon : actor->getWeapons()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(1, 1, 1, 1), "Weapon %d", weaponNumber++);
            ImGui::TableNextColumn();

            drawEquippedWeapon(graphicsContext, weapon);
        }

        ImGui::EndTable();
    }
}

void ActorPanel::drawEquippedItem(GraphicsContext& graphicsContext, Item* item) {
    auto texture = graphicsContext.getTextureLoader().loadTexture(item->getTextureId())->getSDLTexture();
    auto selectableLabel = "##SelectableEquipment" + std::to_string(item->getId());

    Equippable<Stats::GearStats>::Slot slot = Equippable<Stats::GearStats>::getSlotFromName(item->getType());

    ImGui::SetNextItemAllowOverlap();
    ImGui::Selectable(selectableLabel.c_str());
    if(ImGui::BeginPopupContextItem()) {
        if(ImGui::Button("Unequip")) {
            onUnequipClicked(item, slot);
        }
        if(ImGui::Button("Examine")) {
            onExamineClicked(item);
        }

        ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::Image((void*) texture, ImVec2(20, 20));
    ImGui::SameLine();
    ImGui::TextColored(ItemRarityColours.at(item->getRarity()), "[%s]", item->getName().c_str());
}

void ActorPanel::drawEquippedWeapon(GraphicsContext& graphicsContext, Weapon* weapon) {
    auto item = weapon->getItem();
    auto texture = graphicsContext.getTextureLoader().loadTexture(item->getTextureId())->getSDLTexture();
    auto selectableLabel = "##SelectableWeapon" + weapon->getId().getString();

    ImGui::SetNextItemAllowOverlap();
    ImGui::Selectable(selectableLabel.c_str());
    if(ImGui::BeginPopupContextItem()) {
        if(ImGui::Button("Unequip")) {
            onUnequipWeaponClicked(weapon);
        }
        if(ImGui::Button("Examine")) {
            onExamineClicked(item);
        }

        ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::Image((void*) texture, ImVec2(20, 20));
    ImGui::SameLine();
    ImGui::TextColored(ItemRarityColours.at(item->getRarity()), "[%s]", item->getName().c_str());
}

void ActorPanel::addOnUnequipCallback(std::function<void(Item* item, Equippable<Stats::GearStats>::Slot slot)>&& callback) {
    this->onUnequipClicked = callback;
}

void ActorPanel::addOnUnequipWeaponCallback(std::function<void(Weapon* weapon)>&& callback) {
    this->onUnequipWeaponClicked = callback;
}

void ActorPanel::addOnExamineCallback(std::function<void(Item* item)>&& callback) {
    this->onExamineClicked = callback;
}

bool ActorPanel::getIsOpen(void) const {
    return isOpen;
}

void ActorPanel::onPublish(const Event<ActorUpdateStatsEventData>& event) {
    if(event.data.actor->getId() != actor->getId()) {
        return;
    }

    stats = Stats::calculateActorStatCategories(actor->getStats());
}