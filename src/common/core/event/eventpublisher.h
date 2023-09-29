#pragma once

#include <vector>

#include "event.h"
#include "eventsubscriber.h"

template<typename T>
class EventPublisher {
private:
    std::vector<EventSubscriber<T>*> subscribers;

    Event<T> constructEvent(const T& data) {
        Event<T> event;
        event.data = data;
        event.timestamp = std::time(nullptr);
        return event;
    }

public:
    void publish(const T& data) {
        auto const& event = constructEvent(data);

        for(auto subscriber : subscribers) {
            subscriber->onPublish(event);
        }
    }
    
    void subscribe(EventSubscriber<T>* subscriber) {
        subscribers.push_back(subscriber); 
    }
};