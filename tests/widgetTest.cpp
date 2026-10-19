#include "GooseUI/GooseUI.h"

#include "GooseUI/abstractions/iWindow.h"

#include "GooseUI/events/eventLoop.h"

#include "GooseUI/widgets/boxButton.h"


int main()
{   
    GooseUI::event::dispatcher _dispatcher;
    
    GooseUI::absractions::iWindow* _ctrlWindow;
    GooseUI::absractions::iWindow* _testWindow;

    GooseUI::widgets::boxButton* _toggleVisibilityButton;
    GooseUI::widgets::boxButton* _toggleStateButton;
    GooseUI::widgets::boxButton* _toggleSizeButton;
    GooseUI::widgets::boxButton* _toggleResizeablityButton;
    GooseUI::widgets::boxButton* _restoreStateButton;

    GooseUI::application::init(GooseUI::application::backendType::OpenGL);

    // Control Window
    {
        GooseUI::windowCreationInfo winInfo {};
        winInfo.windowTitle = "Control Window";
        winInfo.width = 500;
        winInfo.height = 500;
        winInfo.posistion = GooseUI::SCREEN_LEFT;

        GooseUI::titlebarCreationInfo winBarInfo {};
        winBarInfo.evtDispatcher = &_dispatcher;
        winBarInfo.windowTitle = "Control Window";
        winBarInfo.type = GooseUI::windowDecoration::ServerSide;

        _ctrlWindow = GooseUI::absractions::createWindow(winInfo);
        _ctrlWindow->setTitleBarDecorations(winBarInfo);
        _ctrlWindow->isResizeable(false);
        _ctrlWindow->show();

        // Widgets
        {
            GooseUI::boxButtonCreationInfo buttonInfo {};
            buttonInfo.eventID = 1;
            buttonInfo.evtDispatcher = &_dispatcher;

            buttonInfo.alignment = GooseUI::ALIGN_RIGHT | GooseUI::ALIGN_TOP | GooseUI::ALIGN_BOTTOM | GooseUI::ALIGN_LEFT;
            buttonInfo.X = 10;
            buttonInfo.Y = 10;

            buttonInfo.scaleing = GooseUI::SCALE_ALL;
            buttonInfo.width = 20;
            buttonInfo.height = 20;

            _toggleVisibilityButton = GooseUI::widgets::createBoxButton(buttonInfo);
            _toggleVisibilityButton->addToWindow(_ctrlWindow);

            _dispatcher.add(1, [&](GooseUI::event::data evt){
                printf("[Window Evt] [ID: 1] - Input Detected \n");
            });
        }

        GooseUI::event::loop::add(_ctrlWindow);
    }

    // Test Window
    {
        GooseUI::windowCreationInfo winInfo {};
        winInfo.windowTitle = "Test Window";
        winInfo.width = 500;
        winInfo.height = 500;
        winInfo.posistion = GooseUI::SCREEN_LEFT;

        GooseUI::titlebarCreationInfo winBarInfo {};
        winBarInfo.evtDispatcher = &_dispatcher;
        winBarInfo.windowTitle = "Test Window";
        winBarInfo.type = GooseUI::windowDecoration::ServerSide;

        _testWindow = GooseUI::absractions::createWindow(winInfo);
        _testWindow->setTitleBarDecorations(winBarInfo);
        _testWindow->isResizeable(false);
        _testWindow->show();

        GooseUI::event::loop::add(_testWindow);
    }

    GooseUI::event::loop::run();
}