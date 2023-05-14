//
// Created by David Lakubu on 09/05/2023.
//

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <list>

//typedef  (EventCallback*)(void);
template <typename EventData>
class BaseEventHandler {
public:
    typedef void (*EventCallback)(void *source, EventData data);

    void operator()(void *source, EventData data){
        for(auto listener : listeners){
            listener(source, data);
        }
    }
    void operator+=(EventCallback callback){
        if(*std::find(listeners.begin(), listeners.end(), callback) == nullptr){
            listeners.push_back(callback);
        }
    }
    void operator-=(EventCallback callback){
        if(*std::find(listeners.begin(), listeners.end(), callback) != nullptr){
            std::remove(listeners.begin(), listeners.end(), callback);
        }
    }
private:
    std::list<EventCallback> listeners;
};
using EventHandler = BaseEventHandler<void*>;

#endif //EVENTHANDLER_H
