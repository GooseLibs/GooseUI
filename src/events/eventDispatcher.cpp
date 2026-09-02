#include "GooseUI/events/eventDispatcher.h"


namespace GooseUI::event // Public
{
    void dispatcher::dispatch(const int& eventID, const event::data& Evt)
    {
        if(dispatcher::EventsObj.count(eventID))
        {
            dispatcher::EventsObj[eventID](Evt);
        }
    }

    void dispatcher::add(const int& eventID, std::function<void(event::data)> EvtCallback){ dispatcher::EventsObj[eventID] = EvtCallback; }
    void dispatcher::remove(const int& eventID){ dispatcher::EventsObj.erase(eventID); }
}