#pragma once

#include <chrono>
#include <thread>
#include <mutex>
#include <random>
#include <vector>

#include "core/glmimport.h"
#include "core/util/gameassert.h"
#include "core/util/randomrolls.h"
#include "../sdlimport.h"
#include "../graphicscontext.h"
#include "actionbutton.h"
#include "game/application/application.h"
#include "game/application/turncontroller.h"
#include "game/net/client/gameclientmessagestransmitter.h"

class Dice {
private:
    int face;
    bool rolling;
    std::vector<int> actionsFromServer;

    std::map<int, std::vector<std::unique_ptr<ActionButton>>> actions;

    TurnController& turnController;
    GameClientMessagesTransmitter& clientMessagesTransmitter;

    std::mutex mtx;
    void rollFunc(int seconds);

public:  
    Dice(
        int face,
        GameClientMessagesTransmitter& clientMessagesTransmitter
    );

    void draw(GraphicsContext& graphicsContext);
    void handleClickEvent(int mouseX, int mouseY);
    void clickAction(int id);

    void roll(int participantId);
    bool isRolling(void) const;

    void setActionsFromServer(const std::vector<int>& actionsFromServer);
};