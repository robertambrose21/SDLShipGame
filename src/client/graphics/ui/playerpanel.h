#pragma once

#include <sstream>
#include <format>

#include "textpanel.h"
#include "game/application/turncontroller.h"
#include "game/entities/entitypool.h"
#include "core/event/eventsubscriber.h"

class PlayerPanel : 
    public EventSubscriber<TurnEventData>,
    public EventSubscriber<EntityEventData>,
    public EventSubscriber<WeaponEventData>,
    public EventSubscriber<ProjectileEventData>,
    public EventSubscriber<AreaOfEffectEventData>,
    public EventSubscriber<ItemEventData>,
    public EventSubscriber<TakeItemActionEventData>,
    public EventSubscriber<EngagementEventData>
{
private:
    const int PanelHeight = 200;

    std::unique_ptr<TextPanel> panel;

    std::string getEntityIdentifier(Entity* entity);

    template<typename... Args>
    void log(time_t timestamp, std::format_string<Args...> fmt, Args&&... args);

public:
    PlayerPanel(int width, int height);

    void draw(SDL_Renderer* renderer);
    void onPublish(const Event<TurnEventData>& event);
    void onPublish(const Event<EntityEventData>& event);
    void onPublish(const Event<WeaponEventData>& event);
    void onPublish(const Event<ProjectileEventData>& event);
    void onPublish(const Event<AreaOfEffectEventData>& event);
    void onPublish(const Event<ItemEventData>& event);
    void onPublish(const Event<TakeItemActionEventData>& event);
    void onPublish(const Event<EngagementEventData>& event);
};