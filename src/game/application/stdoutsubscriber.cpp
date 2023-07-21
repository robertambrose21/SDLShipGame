#include "stdoutsubscriber.h"

StdOutSubscriber::StdOutSubscriber()
{ }

void StdOutSubscriber::onPublish(const Event<TurnControllerEventData>& event) {
    std::cout 
        << std::put_time(std::localtime(&event.timestamp), "[%H:%M:%S] Turn number: ")
        << event.data.turnNumber
        << std::endl;
}