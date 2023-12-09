#pragma once

#include <sstream>

#include "textpanel.h"
#include "game/application/turncontroller.h"
#include "game/entities/entitypool.h"
#include "core/event/eventsubscriber.h"

class PlayerPanel : 
    public EventSubscriber<TurnEventData>,
    public EventSubscriber<EntityEventData>,
    public EventSubscriber<WeaponEventData>,
    public EventSubscriber<ProjectileEventData>,
    public EventSubscriber<AreaOfEffectEventData>
{
private:
    const int PanelHeight = 200;

    std::unique_ptr<TextPanel> panel;

    std::string getEntityIdentifier(Entity* entity);

public:
    PlayerPanel(int width, int height);

    void draw(SDL_Renderer* renderer);
    void onPublish(const Event<TurnEventData>& event);
    void onPublish(const Event<EntityEventData>& event);
    void onPublish(const Event<WeaponEventData>& event);
    void onPublish(const Event<ProjectileEventData>& event);
    void onPublish(const Event<AreaOfEffectEventData>& event);
};