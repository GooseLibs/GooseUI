#ifndef _GOOSEUI_TYPES_H_
#define _GOOSEUI_TYPES_H_

namespace GooseUI
{
    namespace absractions
    {
        class iWindow;      //FORWARD DECLERATION
        class iWidget;      //FORWARD DECLERATION
        class iRenderer;    //FORWARD DECLERATION
        class iFont;        //FORWARD DECLERATION
    }

    namespace event
    {
        class dispatcher;   //FORWARD DECLERATION
    }

    namespace graphics 
    {
        struct titleBar;    //FORWARD DECLERATION
    }
        
    enum class displayService
    {
        win32,
        x11,
        wayland
    };

    enum class windowDecoration
    {
        ServerSide,
        ClientSide
    };
    
    enum screenPosistion
    {
        SCREEN_TOP,
        SCREEN_BOTTOM,
        SCREEN_LEFT,
        SCREEN_RIGHT,
        SCREEN_TOP_LEFT,
        SCREEN_TOP_RIGHT,
        SCREEN_BOTTOM_LEFT,
        SCREEN_BOTTOM_RIGHT,
        SCREEN_CENTER
    };
    
    enum widgetScaleing
    {
        SCALE_NONE,
        SCALE_VERTICAL,
        SCALE_HORIZONTAL,
        SCALE_ALL
    };
    
    enum widgetAlignment
    {
        ALIGN_TOP = 1 << 0,
        ALIGN_BOTTOM = 1 << 1,
        ALIGN_LEFT = 1 << 2,
        ALIGN_RIGHT = 1 << 3
    };
    
    struct layoutRestraints
    {
        int minWidth = 0;
        int minHeight = 0;
        int maxWidth = 0;
        int maxHeight = 0;
        
        int minX = 0;
        int minY = 0;
        int maxX = 0;
        int maxY = 0;
    };
    
    struct color 
    { 
        float 
        R, 
        G, 
        B, 
        A; 
    };
}

#endif /*_GOOSEUI_TYPES_H_*/