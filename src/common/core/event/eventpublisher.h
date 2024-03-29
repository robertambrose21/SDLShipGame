#pragma once

#include <vector>
#include <tuple>

#include "event.h"
#include "eventsubscriber.h"

template<typename... Types>
class EventPublisher {
private:
    std::tuple<std::vector<EventSubscriber<Types>*>...> subscribers;

    template<typename T>
    Event<T> constructEvent(const T& data) {
        Event<T> event;
        event.data = data;
        event.timestamp = std::time(nullptr);
        return event;
    }

public:
    template<typename T>
    void publish(const T& data) {
        auto const& event = constructEvent(data);

        for(auto subscriber : std::get<std::vector<EventSubscriber<T>*>>(subscribers)) {
            subscriber->onPublish(event);
        }
    }
    
    template<typename T>
    void subscribe(EventSubscriber<T>* subscriber) {
        std::get<std::vector<EventSubscriber<T>*>>(subscribers).push_back(subscriber); 
    }
};
