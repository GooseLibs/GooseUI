#ifndef _GOOSEUI_EVENTDISPATCHER_H_
#define _GOOSEUI_EVENTDISPATCHER_H_

#include "GooseUI/events/event.h"

#include <unordered_map>
#include <functional>

namespace GooseUI
{
    namespace event
    {
        class dispatcher
        {
            std::unordered_map<int, std::function<void(event::data)>> EventsObj;

            public:
            void add(const int& eventID, std::function<void(event::data)> EvtCallback);
            void remove(const int& eventID);
            
            void dispatch(const int& eventID, const event::data& Evt);
        };
    }
}

#endif /*_GOOSEUI_EVENTDISPATCHER_H_*/