#pragma once

#include <vector>
#include <tuple>

#include "event.h"
#include "eventsubscriber.h"

template<typename... Types>
class EventPublisher {
public:
    template<typename T>
    void publish(const T& data) {
        auto const& event = constructEvent(data);

        for(auto const& subscriber : std::get<std::vector<EventSubscriber<T>*>>(subscribers)) {
            subscriber->onPublish(event);
        }
    }
    
    template<typename T>
    void subscribe(EventSubscriber<T>* subscriber) {
        std::get<std::vector<EventSubscriber<T>*>>(subscribers).push_back(subscriber); 
    }

    template<typename T>
    void unsubscribe(EventSubscriber<T>* subscriber) {
        auto& v = std::get<std::vector<EventSubscriber<T>*>>(subscribers);
        v.erase(std::remove(v.begin(), v.end(), subscriber), v.end());
    }

private:
    std::tuple<std::vector<EventSubscriber<Types>*>...> subscribers;

    template<typename T>
    Event<T> constructEvent(const T& data) {
        Event<T> event;
        event.data = data;
        event.timestamp = std::time(nullptr);
        return event;
    }
};
