#pragma once
#include "Core/RobotMap.h"
#include <vector>
#include <unordered_map>
#include <map>
#include <typeindex>
#include <list>

class Event {
    Entity publisher;
};
class ToggleSpinEvent : public Event {
};
class EventManager {

    private:
        class FunctionHandlerBase;
        typedef std::list<FunctionHandlerBase*> HandlerList;
    public:
        template<class T, class EventType>
        void subscribe(T* instance, void (T::*fcnPtr) (EventType*)){
            auto handlers = subscribers[typeid(EventType)];

            //If this is the first subscriber of this type
            if(handlers == nullptr) {
                handlers = new HandlerList();
                subscribers[typeid(EventType)] = handlers;
            }
            handlers->push_back(new FunctionHandler<T, EventType>(instance,fcnPtr));
        }

        template<typename EventType>
        void postEvent(EventType* event) {
            HandlerList* handlers = subscribers[typeid(EventType)];

            if(handlers == nullptr) {
                return;
            }

            for(auto& handler : *handlers) {
                if(handler != nullptr) {
                    handler->call(event);
                }
            }
        }
    private:
        //Helper Classes
        class FunctionHandlerBase {
            public:
                virtual void call(Event* event) = 0;
        };
        template<class T, class EventType>
        class FunctionHandler : public FunctionHandlerBase {
            public:
                FunctionHandler(T* instance, void (T::*function)(EventType*)) : function{function}, instance{instance} {};
                void call(Event* event) {
                    (instance->*function)(static_cast<EventType*>(event));
                }
            private:
                void (T::*function)(EventType*);
                T* instance;
        };

        //List of Functions
        std::map<std::type_index, HandlerList*> subscribers;
};
