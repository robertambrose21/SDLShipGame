#pragma once

#include <chrono>
#include <thread>
#include <mutex>
#include <random>
#include <vector>

#include "core/glmimport.h"
#include "core/util/gameassert.h"
#include "core/util/randomrolls.h"
#include "sdlimport.h"
#include "graphicscontext.h"
#include "button.h"
#include "game/application/application.h"
#include "game/application/turncontroller.h"
#include "game/net/client/gameclientmessagestransmitter.h"

class Dice {
private:
    int face;
    bool rolling;
    std::vector<int> actionsFromServer;

    std::map<int, std::vector<std::shared_ptr<Button>>> actions;

    std::shared_ptr<TurnController> turnController;
    std::shared_ptr<GameClientMessagesTransmitter> clientMessagesTransmitter;

    std::mutex mtx;
    void rollFunc(int seconds);

public:  
    Dice(
        int face,
        std::shared_ptr<GameClientMessagesTransmitter> clientMessagesTransmitter
    );

    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);
    void handleClickEvent(int mouseX, int mouseY);
    void removeAction(int id);

    void roll(int participantId);
    bool isRolling(void) const;

    void setActionsFromServer(const std::vector<int>& actionsFromServer);
};