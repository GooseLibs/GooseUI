#ifndef _GOOSEUI_EVENT_H_
#define _GOOSEUI_EVENT_H_

namespace GooseUI
{
    namespace event
    {
        enum class type
        {
            none,
            leftMouseDown,
            leftMouseUp,
            rightMouseDown
        };
        
        struct data
        {
            type dataType = type::none;

            float mouseX = 0;
            float mouseY = 0;

            float mouseRootX = 0;
            float mouseRootY = 0;
        };
    }
}

#endif /*_GOOSEUI_EVENT_H_*/