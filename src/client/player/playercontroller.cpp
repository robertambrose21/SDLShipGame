#include "playercontroller.h"

// TODO: Disengage if the player is out of aggro range
PlayerController::PlayerController(
    GameClientMessagesTransmitter& clientMessagesTransmitter,
    ApplicationContext& context,
    GraphicsContext& graphicsContext
) :
    clientMessagesTransmitter(clientMessagesTransmitter),
    graphicsContext(graphicsContext),
    gridRenderer(graphicsContext.getGridRenderer()),
    gameController(context.getGameController()),
    actorPool(context.getActorPool()),
    grid(context.getGrid()),
    camera(graphicsContext.getGridRenderer().getCamera()),
    isLeftShiftPressed(false),
    isCurrentWeaponInRange(true),
    cameraVector(glm::ivec2(0, 0)),
    participant(nullptr)
{
    playerPanel = std::make_unique<PlayerPanel>(1920, 1080);
    inventoryPanel = std::make_unique<InventoryPanel>(400, 600);
    diagnosticsPanel = std::make_unique<DiagnosticsPanel>();

    inventoryPanel->addOnEquipCallback([&](auto item, auto slot) {
        equipItem(item, slot);
    });
    inventoryPanel->addOnEquipWeaponCallback([&](auto item) {
        equipWeapon(item);
    });
    inventoryPanel->addOnExamineCallback([&](auto item) {
        if(!examineItemPanels.contains(item->getId())) {
            examineItemPanels[item->getId()] = std::make_unique<ExamineItemPanel>(item);
        }
    });
    
    actorPool->subscribe<ActorEventData>(playerPanel.get());
    context.getWeaponController()->subscribe<MeleeWeaponEventData>(playerPanel.get());
    context.getProjectilePool()->subscribe<ProjectileEventData>(playerPanel.get());
    context.getAreaOfEffectPool()->subscribe<AreaOfEffectEventData>(playerPanel.get());
    context.getItemController()->subscribe<ItemEventData>(playerPanel.get());
    context.getGameController()->subscribe<TakeItemActionEventData>(playerPanel.get());
}

void PlayerController::update(int64_t timeSinceLastFrame) {
    for(auto actor : selectedActors) {
        if(actor == nullptr || actor->getCurrentHP() <= 0) {
            selectedActors.erase(
                std::remove(selectedActors.begin(), selectedActors.end(), actor), selectedActors.end());
        }
    }

    setHoverTiles();

    if(selectedActors.size() == 1) {
        auto windowWidth = graphicsContext.getWindowWidth();
        auto windowHeight = graphicsContext.getWindowHeight();
        auto position = selectedActors[0]->getPosition();
        
        camera.setPosition(gridRenderer.getTilePosition(-position.x, -position.y) +
            glm::ivec2(windowWidth / 2, windowHeight / 2));
    }
    else {
        camera.move(cameraVector * (int) timeSinceLastFrame);
    }

    diagnosticsPanel->update(timeSinceLastFrame);
}

void PlayerController::draw(GraphicsContext& graphicsContext) {
    if(selection.isActive) {
        auto size = selection.end - selection.start;

        SDL_Rect rect = { selection.start.x, selection.start.y, size.x, size.y };
        SDL_SetRenderDrawColor(graphicsContext.getRenderer(), 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderDrawRect(graphicsContext.getRenderer(), &rect);
    }

    for(auto tile : hoverTiles) {
        auto const& realPosition = gridRenderer.getTilePosition(tile.x, tile.y) + camera.getPosition();
        SDL_Rect dst = { realPosition.x, realPosition.y, gridRenderer.getTileSize(), gridRenderer.getTileSize() };
        SDL_SetRenderDrawBlendMode(graphicsContext.getRenderer(), SDL_BLENDMODE_BLEND);
        if(isCurrentWeaponInRange) {
            SDL_SetRenderDrawColor(graphicsContext.getRenderer(), 0x00, 0xFF, 0x00, 0x7F);
        }
        else {
            SDL_SetRenderDrawColor(graphicsContext.getRenderer(), 0xFF, 0x00, 0x00, 0x7F);
        }
        SDL_RenderFillRect(graphicsContext.getRenderer(), &dst);
        SDL_RenderDrawLine(graphicsContext.getRenderer(), p1.x, p1.y, p2.x, p2.y);
    }
}

void PlayerController::drawUI(GraphicsContext& graphicsContext) {
    playerPanel->draw();
    inventoryPanel->draw(graphicsContext, participant);
    diagnosticsPanel->draw();

    std::erase_if(examineItemPanels, [](const auto& item) {
        auto const& [_, examineItemPanel] = item;
        return !examineItemPanel->getIsOpen();
    });

    std::erase_if(actorPanels, [&](const auto& item) {
        auto const& [_, actorPanel] = item;
        bool isOpen = actorPanel->getIsOpen();

        if(!isOpen) {
            actorPool->unsubscribe<ActorUpdateStatsEventData>(actorPanel.get());
        }

        return !isOpen;
    });

    for(auto& [_, examineItemPanel] : examineItemPanels) {
        examineItemPanel->draw(graphicsContext);
    }

    for(auto& [_, actorPanel] : actorPanels) {
        actorPanel->draw(graphicsContext);
    }
}

void PlayerController::handleKeyPress(const SDL_Event& event) {
    if(event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        switch(event.key.keysym.sym) {
            case SDLK_p: {
                clientMessagesTransmitter.sendPassParticipantTurnMessage(participant->getId());
                break;
            }
            
            case SDLK_i: {
                inventoryPanel->toggle();
                break;
            }

            case SDLK_c: {
                for(auto actor : selectedActors) {
                    addActorPanel(actor);
                }
                break;
            }

            case SDLK_LSHIFT:
                isLeftShiftPressed = true;
                break;

            case SDLK_RIGHT:
                cameraVector += glm::ivec2(-1, 0);
                break;

            case SDLK_LEFT:
                cameraVector += glm::ivec2(1, 0);
                break;

            case SDLK_UP:
                cameraVector += glm::ivec2(0, 1);
                break;

            case SDLK_DOWN:
                cameraVector += glm::ivec2(0, -1);
                break;

            default:
                break;
        }
    }
    if(event.type == SDL_KEYUP) {
        switch(event.key.keysym.sym) {
            case SDLK_LSHIFT:
                isLeftShiftPressed = false;
                break;

            case SDLK_RIGHT:
                cameraVector -= glm::ivec2(-1, 0);
                break;

            case SDLK_LEFT:
                cameraVector -= glm::ivec2(1, 0);
                break;

            case SDLK_UP:
                cameraVector -= glm::ivec2(0, 1);
                break;

            case SDLK_DOWN:
                cameraVector -= glm::ivec2(0, -1);
                break;

            default:
                break;
        }
    }
}

void PlayerController::handleMouseEvent(const SDL_Event& event) {
    auto mousePosition = glm::ivec2(event.button.x, event.button.y);
    auto [x, y] = gridRenderer.getTileIndices(mousePosition - camera.getPosition());

    switch(event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            handleMouseDown(event);
            break;
        }
        
        case SDL_MOUSEBUTTONUP: {
            handleMouseUp(event);
            break;
        }

        case SDL_MOUSEMOTION: {
            if(selection.isActive) {
                selection.end = glm::ivec2(event.motion.x, event.motion.y);
            }

            break;
        }

        default:
            break;
    }
}

void PlayerController::handleMouseDown(const SDL_Event& event) {
    auto mousePosition = glm::ivec2(event.button.x, event.button.y);
    auto [x, y] = gridRenderer.getTileIndices(mousePosition - camera.getPosition());
    auto position = glm::ivec2(x, y);

    switch(event.button.button) {
        case SDL_BUTTON_LEFT: {
            if(isLeftShiftPressed) {
                attack(position);
                break;
            }

            auto actor = Actor::filterByTile(x, y, participant->getActors());

            if(actor != nullptr) {
                toggleSelection({ actor });
            }
            else {
                selection.isActive = true;
                selection.start = mousePosition;
                selection.end = mousePosition;

                deselectAll();
            }
            break;
        }

        case SDL_BUTTON_RIGHT: {
            auto const& target = Actor::filterByTile(x, y, actorPool->getActors());

            if(target != nullptr || isLeftShiftPressed) {
                attack(position);
            }
            else {
                move(glm::ivec2(x, y));
            }
            
            break;
        }
    }
}

void PlayerController::handleMouseUp(const SDL_Event& event) {
    auto grid = gridRenderer.getGrid();

    switch(event.button.button) {
        case SDL_BUTTON_LEFT: {
            if(selection.isActive) {
                selection.isActive = false;

                auto start = glm::ivec2(
                    std::min(selection.start.x, selection.end.x),
                    std::min(selection.start.y, selection.end.y)
                ) - camera.getPosition();
                auto end = glm::ivec2(
                    std::max(selection.start.x, selection.end.x),
                    std::max(selection.start.y, selection.end.y)
                ) - camera.getPosition();

                auto [x, y] = gridRenderer.getTileIndices(start);
                auto [sizeX, sizeY] = gridRenderer.getTileIndices(end - start);

                auto tiles = gridRenderer.getGrid()->getTilesInSquare(x, y, sizeX, sizeY);
                
                toggleSelection(Actor::filterByTiles(tiles, participant->getActors()));
            }
            break;
        }
    }
}

void PlayerController::toggleSelection(const std::vector<Actor*>& actors) {
    if(actors.empty()) {
        return;
    }

    if(actors.size() == 1 && actors[0] != nullptr && actors[0]->isSelected()) {
        selectedActors.erase(std::find(selectedActors.begin(), selectedActors.end(), actors[0]));
        actors[0]->setSelected(false);
        return;
    }

    for(auto actor : actors) {
        clientMessagesTransmitter.sendSelectActorMessage(actor->getId());
        selectedActors.push_back(actor);
        actor->setSelected(true);
    }
}

void PlayerController::selectAll(void) {
    toggleSelection(participant->getActors());
}

void PlayerController::deselectAll(void) {
    for(auto actor : selectedActors) {
        actor->setSelected(false);
    }

    selectedActors.clear();
}

void PlayerController::move(const glm::ivec2& position) {
    int turnNumber = participant->hasAnyEngagement() ? participant->getEngagement()->getTurnNumber() : -1;

    for(auto const& actor : selectedActors) {
        if(!grid->findPath(actor->getPosition(), position).empty()) {
            clientMessagesTransmitter.sendFindPathMessage(actor->getId(), position, 0, turnNumber);
        }
    }
}

void PlayerController::attack(const glm::ivec2& target) {
    int turnNumber = participant->hasAnyEngagement() ? participant->getEngagement()->getTurnNumber() : -1;

    for(auto const& actor : selectedActors) {
        auto const& weapon = actor->getCurrentWeapon();
        
        // TODO: ClientGameController actions
        if(doAction(
            std::make_unique<AttackAction>(
                participant, 
                actor, 
                weapon, 
                target, 
                true
            ))
        ) {
            clientMessagesTransmitter.sendAttackMessage(
                actor->getId(), 
                target, 
                weapon->getId(),
                turnNumber
            );
        }
    }
}

void PlayerController::setHoverTiles(void) {
    if(!isLeftShiftPressed || selectedActors.empty()) {
        hoverTiles.clear();
        return;
    }

    auto actor = selectedActors[0];
    auto weapon = actor->getCurrentWeapon();

    if(weapon == nullptr || weapon->getType() != Stats::WeaponStats::PROJECTILE) {
        return;
    }

    glm::ivec2 mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
    auto [x, y] = gridRenderer.getTileIndices(mousePosition - camera.getPosition());

    auto grid = gridRenderer.getGrid();

    isCurrentWeaponInRange = weapon->isInRange(glm::vec2(x, y));
    p1 = (actor->getPosition() * 32) + glm::ivec2(16, 16) + camera.getPosition();
    p2 = (glm::ivec2(x, y) * 32) + glm::ivec2(16, 16) + camera.getPosition();

    if(weapon->getName() == "Grenade Launcher") {
        hoverTiles = gridRenderer.getGrid()->getTilesInCircle(x, y, 2);
    }
    else if(weapon->getName()== "Freeze Gun") {
        glm::ivec2 dir = actor->getPosition() - glm::ivec2(x, y);
        auto perp = glm::normalize(glm::vec2(dir.y, -dir.x));
        auto pX = std::min(grid->getWidth() - 1, (int) std::round(perp.x));
        auto pY = std::min(grid->getHeight() - 1, (int) std::round(perp.y));

        hoverTiles = {
            glm::ivec2(x, y),
            glm::ivec2(x + pX, y + pY),
            glm::ivec2(x - pX, y - pY)
        };
    }
}

void PlayerController::equipItem(Item* item, Equippable<Stats::GearStats>::Slot slot) {
    auto actor = selectedActors[0];

    if(doAction(
        std::make_unique<EquipGearAction>(
            participant, 
            actor, 
            item, 
            slot, 
            false
        ))
    ) {
        clientMessagesTransmitter.sendEquipItemMessage(item->getId(), actor->getId(), slot, false);
    }
}

void PlayerController::unequipItem(Item* item, Equippable<Stats::GearStats>::Slot slot) {
    auto actor = selectedActors[0];

    if(doAction(
        std::make_unique<EquipGearAction>(
            participant, 
            actor, 
            item, 
            slot, 
            true
        ))
    ) {
        clientMessagesTransmitter.sendEquipItemMessage(item->getId(), actor->getId(), slot, true);
    }
}

void PlayerController::equipWeapon(Item* item) {
    auto actor = selectedActors[0];
    auto weaponId = UUID::getNewUUID();

    if(doAction(
        std::make_unique<EquipWeaponAction>(
            participant, 
            actor, 
            item, 
            weaponId,
            false
        ))
    ) {
        spdlog::trace("Player equipping weapon {} to actor {}", weaponId.getString(), actor->getId());
        clientMessagesTransmitter.sendEquipWeaponMessage(item->getId(), actor->getId(), weaponId, false);
    }
}

void PlayerController::unequipWeapon(Weapon* weapon) {
    auto actor = selectedActors[0];
    auto weaponId = weapon->getId();
    auto itemId = weapon->getItem()->getId();

    if(doAction(
        std::make_unique<EquipWeaponAction>(
            participant,
            actor,
            weapon->getItem(),
            weaponId,
            true
        ))
    ) {
        spdlog::trace("Player unequipping weapon {} from actor {}", weaponId.getString(), actor->getId());
        clientMessagesTransmitter.sendEquipWeaponMessage(itemId, actor->getId(), weaponId, true);
    }
}

bool PlayerController::doAction(std::unique_ptr<Action> action) {
    if(participant->hasAnyEngagement()) {
        return gameController->queueAction(std::move(action));
    }


    return gameController->executeActionImmediately(std::move(action));
}

const std::vector<Actor*>& PlayerController::getSelectedActors(void) const {
    return selectedActors;
}

void PlayerController::addActorPanel(Actor* actor) {
    if(actor == nullptr) {
        return;
    }

    if(actorPanels.contains(actor->getId())) {
        return;
    }

    auto panel = std::make_unique<ActorPanel>(400, 400, actor);

    panel->addOnUnequipCallback([&](auto item, auto slot) {
        unequipItem(item, slot);
    });

    panel->addOnUnequipWeaponCallback([&](auto weapon) {
        unequipWeapon(weapon);
    });

    panel->addOnExamineCallback([&](auto item) {
        if(!examineItemPanels.contains(item->getId())) {
            examineItemPanels[item->getId()] = std::make_unique<ExamineItemPanel>(item);
        }
    });

    actorPool->subscribe<ActorUpdateStatsEventData>(panel.get());

    actorPanels[actor->getId()] = std::move(panel);
}

void PlayerController::setParticipant(Participant* participant) {
    game_assert(participant != nullptr);
    game_assert(participant->getIsPlayer());
    this->participant = participant;
    gridRenderer.setParticipant(participant);
}

Participant* PlayerController::getParticipant(void) {
    return participant;
}

PlayerPanel* PlayerController::getPlayerPanel(void) {
    return playerPanel.get();
}
