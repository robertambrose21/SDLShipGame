#pragma once

#include <sstream>
#include <format>
#include <vector>

#include "imgui.h"
#include "itemraritycolours.h"

#include "game/application/turncontroller.h"
#include "game/entities/entitypool.h"
#include "core/event/eventsubscriber.h"

class PlayerPanel : 
    public EventSubscriber<TurnEventData>,
    public EventSubscriber<EntityEventData>,
    public EventSubscriber<MeleeWeaponEventData>,
    public EventSubscriber<ProjectileEventData>,
    public EventSubscriber<AreaOfEffectEventData>,
    public EventSubscriber<ItemEventData>,
    public EventSubscriber<TakeItemActionEventData>
{
private:
    typedef struct _textSegment {
        std::string text;
        ImVec4 colour;
    } TextSegment;

    const int PanelHeight = 200;
    const ImVec4 TimestampColour = ImVec4(.8f, .8f, .7f, 1);
    const ImVec4 StdTextColour = ImVec4(.8f, .8f, .8f, 1);
    const ImVec4 HighlightColour = ImVec4(1, 1, 1, 1);

    int width;
    int height;

    std::vector<std::vector<TextSegment>> lines;

    std::string getEntityIdentifier(Entity* entity);
    
    std::string getTimestampString(std::time_t timestamp);
    void appendItemsToLine(std::vector<TextSegment>& segment, const std::vector<Item*>& items);

public:
    PlayerPanel(int width, int height);

    void draw(void);

    void onPublish(const Event<TurnEventData>& event);
    void onPublish(const Event<EntityEventData>& event);
    void onPublish(const Event<MeleeWeaponEventData>& event);
    void onPublish(const Event<ProjectileEventData>& event);
    void onPublish(const Event<AreaOfEffectEventData>& event);
    void onPublish(const Event<ItemEventData>& event);
    void onPublish(const Event<TakeItemActionEventData>& event);
};