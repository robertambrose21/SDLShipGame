#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <iostream>
#include <random>

#include "application/net/gameclientmessagestransmitter.h"
#include "game/application/application.h"
#include "graphics/gridrenderer.h"
#include "graphics/textureloader.h"
#include "game/entities/entity.h"
#include "game/entities/entitypool.h"
#include "game/weapons/weapon.h"
#include "game/application/turncontroller.h"
#include "graphics/ui/dice.h"
#include "graphics/ui/button.h"
#include "graphics/ui/playerpanel.h"
#include "graphics/ui/inventorypanel.h"
#include "graphics/ui/examineitempanel.h"
#include "graphics/ui/entitypanel.h"
#include "graphics/ui/diagnosticspanel.h"
#include "game/actions/attackaction.h"
#include "game/actions/equipgearaction.h"
#include "game/actions/equipweaponaction.h"

// Too large, split up
class PlayerController {
private:
    typedef struct _selection {
        glm::ivec2 start, end;
        bool isActive;

        _selection() :
            isActive(false)
        { }
    } Selection;

    GameClientMessagesTransmitter& clientMessagesTransmitter;

    Participant* participant;

    std::vector<Entity*> selectedEntities;
    GridRenderer& gridRenderer;
    TurnController* turnController;
    EntityPool* entityPool;
    Grid* grid;
    GraphicsContext& graphicsContext;

    bool isLeftShiftPressed;

    Camera& camera;
    glm::ivec2 cameraVector;
    Selection selection;
    std::vector<glm::ivec2> hoverTiles;
    bool isCurrentWeaponInRange;
    glm::ivec2 p1, p2;

    std::unique_ptr<PlayerPanel> playerPanel;
    std::unique_ptr<InventoryPanel> inventoryPanel;
    std::unique_ptr<DiagnosticsPanel> diagnosticsPanel;
    std::map<uint32_t, std::unique_ptr<ExamineItemPanel>> examineItemPanels;
    std::map<uint32_t, std::unique_ptr<EntityPanel>> entityPanels;

    void move(const glm::ivec2& position);
    void attack(const glm::ivec2& target);
    void handleMouseDown(const SDL_Event& event);
    void handleMouseUp(const SDL_Event& event);
    void setHoverTiles(void);
    void equipItem(Item* item, Equippable<Stats::GearStats>::Slot slot);
    void unequipItem(Item* item, Equippable<Stats::GearStats>::Slot slot);
    void equipWeapon(Item* item);
    void unequipWeapon(Weapon* weapon);

public:
    PlayerController(
        GameClientMessagesTransmitter& clientMessagesTransmitter,
        ApplicationContext& context,
        GraphicsContext& graphicsContext
    );

    void update(int64_t timeSinceLastFrame);
    void draw(GraphicsContext& graphicsContext);
    void drawUI(GraphicsContext& graphicsContext);

    void handleKeyPress(const SDL_Event& event);
    void handleMouseEvent(const SDL_Event& event);

    const std::vector<Entity*>& getSelectedEntities(void) const;
    void setParticipant(Participant* participant);
    Participant* getParticipant(void);

    void toggleSelection(const std::vector<Entity*>& entities);
    void selectAll(void);
    void deselectAll(void);

    void addEntityPanel(Entity* entity);

    PlayerPanel* getPlayerPanel(void);
};
