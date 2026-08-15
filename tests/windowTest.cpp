#include "GooseUI/GooseUI.h"

#include "GooseUI/abstractions/iWindow.h"
#include "GooseUI/events/eventLoop.h"

/*
    The first window WILL ALLWAYS be the parent window, if it is closed all other windows will close and the loop will end
    I could eventually add a way to stop this, but honestly I dont want to, the the bug is a feature now
*/

GooseUI::absractions::iWindow* _window1;
GooseUI::absractions::iWindow* _window2;
GooseUI::event::dispatcher _dispatcher;

int main()
{
    GooseUI::application::init(GooseUI::application::backendType::OpenGL);

    // Window One
    {
        GooseUI::windowCreationInfo window1_info {};
        window1_info.width = 500;
        window1_info.height = 500;
        window1_info.posistion = GooseUI::SCREEN_TOP_RIGHT;

        GooseUI::titlebarCreationInfo window1_titleBarInfo {};
        window1_titleBarInfo.evtDispatcher = &_dispatcher;
        window1_titleBarInfo.windowTitle = "Window One";
        window1_titleBarInfo.type = GooseUI::windowDecoration::ClientSide;

        _window1 = GooseUI::absractions::createWindow(window1_info); 
        _window1->setTitleBarDecorations(window1_titleBarInfo);
        _window1->isResizeable(true);
        _window1->show();

        GooseUI::event::loop::add(_window1);
    }
    
    // Window Two
    {
        GooseUI::windowCreationInfo window2_info {};
        window2_info.width = 800;
        window2_info.height = 500;
        window2_info.posistion = GooseUI::SCREEN_TOP_LEFT;

        GooseUI::titlebarCreationInfo window2_titleBarInfo {};
        window2_titleBarInfo.evtDispatcher = &_dispatcher;
        window2_titleBarInfo.windowTitle = "Window Two";
        window2_titleBarInfo.type = GooseUI::windowDecoration::ClientSide;

        _window2 = GooseUI::absractions::createWindow(window2_info); 
        _window2->setTitleBarDecorations(window2_titleBarInfo);
        _window2->isResizeable(false);
        _window2->show();

        GooseUI::event::loop::add(_window2);
    }
    
    GooseUI::event::loop::run();
}