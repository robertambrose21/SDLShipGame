#include "dice.h"

Dice::Dice(
    int face,
    std::shared_ptr<GameClientMessagesTransmitter> clientMessagesTransmitter
) :
    face(face),
    clientMessagesTransmitter(clientMessagesTransmitter),
    rolling(false)
{
    game_assert(face >= 1 && face <= 6);
    turnController = Application::getContext()->getTurnController();
}

void Dice::draw(const std::shared_ptr<GraphicsContext>& graphicsContext) {
    static uint32_t diceTextureIdOffset = 7;

    SDL_Rect dst = {
        830,
        50,
        64,
        64
    };

    graphicsContext->getTextureLoader()
        ->loadTexture(face + diceTextureIdOffset)
        ->draw(graphicsContext->getRenderer(), NULL, &dst);

    for(auto const& button : actions) {
        button->draw(graphicsContext);
    }
}

void Dice::handleClickEvent(int mouseX, int mouseY) {
    if(isRolling()) {
        return;
    }

    for(auto const& button : actions) {
        button->handleClickEvent(mouseX, mouseY);
    }
}

void Dice::roll(int participantId) {
    if(isRolling()) {
        return;
    }

    actionsFromServer.clear();
    actions.clear();
    
    clientMessagesTransmitter->sendActionsRollMessage(participantId);

    rolling = true;
    std::thread t(&Dice::rollFunc, this, 3);
    t.detach();
}

// https://stackoverflow.com/questions/9094422/how-to-check-if-a-stdthread-is-still-running
// https://cplusplus.com/reference/mutex/mutex/
void Dice::rollFunc(int seconds) {
    auto startTime = std::chrono::system_clock::now();
    auto now = startTime;

    // Will eventually need a retry/timeout
    while(
        actionsFromServer.empty() ||
        (std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() < (seconds - 1) * 1000)
    ) {
        mtx.lock();
        face = randomD6();
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));        
        now = std::chrono::system_clock::now();
    }

    face = actionsFromServer.size();

    int numMoveActions = 0;
    int numAttackActions = 0;

    mtx.lock();
    for(int i = 0; i < face; i++) {
        auto buttonTypeId = actionsFromServer[i];//randomRange(0, 2);

        if(buttonTypeId == 0) {
            auto moveButton = std::make_shared<Button>(14, glm::ivec2{900, (i * 70) + 50}, glm::ivec2{64, 64});
            moveButton->onClick([]{
                std::cout << "Move clicked" << std::endl;
            });
            actions.push_back(moveButton);
            numMoveActions++;
        }
        else if(buttonTypeId == 1) {
            auto shootButton = std::make_shared<Button>(15, glm::ivec2{900, (i * 70) + 50}, glm::ivec2{64, 64});
            shootButton->onClick([]{
                std::cout << "Shoot clicked" << std::endl;
            });
            actions.push_back(shootButton);
            numAttackActions++;
        }
        else if(buttonTypeId == 2) {
            auto freezeButton = std::make_shared<Button>(16, glm::ivec2{900, (i * 70) + 50}, glm::ivec2{64, 64});
            freezeButton->onClick([]{
                std::cout << "Freeze clicked" << std::endl;
            });
            actions.push_back(freezeButton);
        }
    }

    turnController->setActions(0, 
        {
            { TurnController::Action::Move, numMoveActions },
            { TurnController::Action::Attack, numAttackActions }
        }
    );

    mtx.unlock();

    rolling = false;
}

bool Dice::isRolling(void) const {
    return rolling;
}

void Dice::setActionsFromServer(const std::vector<int>& actionsFromServer) {
    this->actionsFromServer = actionsFromServer;
}