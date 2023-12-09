#include "playerpanel.h"

PlayerPanel::PlayerPanel(int width, int height) {
    auto font = Font::loadFromFile("../assets/fonts/RobotoMono-SemiBold.ttf", 14);
    panel = std::make_unique<TextPanel>(std::move(font), glm::ivec2(0, height - PanelHeight), glm::ivec2(width, PanelHeight));
    panel->setColour(0x64, 0x64, 0x64, 0xFF);
}

void PlayerPanel::draw(SDL_Renderer* renderer) {
    panel->draw(renderer);
}

void PlayerPanel::onPublish(const Event<TurnEventData>& event) {
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&event.timestamp), "[%H:%M:%S]: Turn ") << event.data.turnNumber;

    panel->writeLine(oss.str());
}

void PlayerPanel::onPublish(const Event<EntityEventData>& event) {
    std::ostringstream oss;
    auto time = std::put_time(std::localtime(&event.timestamp), "[%H:%M:%S]: ");

    if(event.data.type == "Death") {
        oss 
            << time
            << getEntityIdentifier(event.data.entity)
            << " died.";

        panel->writeLine(oss.str());
    }
    else if(event.data.type == "Freeze" && event.data.entity->getFrozenFor() <= 0) {
        oss
            << time
            << getEntityIdentifier(event.data.entity)
            << " unfreezes.";

        panel->writeLine(oss.str());
    }
}

void PlayerPanel::onPublish(const Event<WeaponEventData>& event) {
    if(event.data.weapon->getType() != Weapon::Type::MELEE) {
        return;
    }

    std::ostringstream oss;
    oss 
        << std::put_time(std::localtime(&event.timestamp), "[%H:%M:%S]: ") 
        << getEntityIdentifier(event.data.owner)
        << " meleed " 
        << getEntityIdentifier(event.data.target)
        << " for "
        << event.data.weapon->getStats().damage
        << " damage! "
        << getEntityIdentifier(event.data.target)
        << " now has "
        << event.data.target->getCurrentHP()
        << " HP.";

    panel->writeLine(oss.str());
}

void PlayerPanel::onPublish(const Event<ProjectileEventData>& event) {
    if(event.data.target == nullptr) {
        return;
    }

    std::ostringstream oss;
    auto time = std::put_time(std::localtime(&event.timestamp), "[%H:%M:%S]: ");

    if(event.data.damage > 0) {
        oss 
            << time
            << getEntityIdentifier(event.data.target)
            << " was hit by a projectile from participant ["
            << event.data.projectile->getOwnerId()
            << "] and took "
            << event.data.damage
            << " damage! "
            << getEntityIdentifier(event.data.target)
            << " now has "
            << event.data.target->getCurrentHP()
            << " HP.";

        panel->writeLine(oss.str());
    }

    auto effects = event.data.projectile->getStats().effects;
    for(auto effect : effects) {
        oss.clear();
        oss.str("");

        if(effect.name == "freeze") {
            oss 
                << time
                << getEntityIdentifier(event.data.target)
                << " is frozen for "
                << effect.duration
                << " turns.";

            panel->writeLine(oss.str());
        }
    }
}

void PlayerPanel::onPublish(const Event<AreaOfEffectEventData>& event) {
    std::ostringstream oss;
    oss 
        << std::put_time(std::localtime(&event.timestamp), "[%H:%M:%S]: ")
        << getEntityIdentifier(event.data.target)
        << " was hit by an area of effect from participant ["
        << event.data.aoe->getOwnerId()
        << "] and took "
        << event.data.aoe->getStats().damagePerTurn
        << " damage! "
        << getEntityIdentifier(event.data.target)
        << " now has "
        << event.data.target->getCurrentHP()
        << " HP.";

    panel->writeLine(oss.str());
}

std::string PlayerPanel::getEntityIdentifier(Entity* entity) {
    game_assert(entity != nullptr);
    return entity->getName() + "#" + std::to_string(entity->getId());
}