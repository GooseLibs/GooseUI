#include "GooseUI/GooseUI.h"

#include "GooseUI/abstractions/iWindow.h"

#include "GooseUI/events/eventLoop.h"

#include "GooseUI/widgets/boxButton.h"


GooseUI::absractions::iWindow* _window;
GooseUI::absractions::iWindow* _window2;
GooseUI::widgets::boxButton* _button;
GooseUI::widgets::boxButton* _button2;
GooseUI::event::dispatcher _dispatcher;

int main()
{   
    _window = GooseUI::absractions::createWindow("Window One", 500, 500, GooseUI::SCREEN_CENTER);
    _window->isResizeable(true);
    _window->setHeader("GooseUI Window", true, true, true, true);

    _window2 = GooseUI::absractions::createWindow("Window Two", 500, 500, GooseUI::SCREEN_CENTER);
    _window2->isResizeable(true);
    _window2->setHeader("GooseUI Window 2", true, true, true, true);

    // Button
    _button = GooseUI::widgets::createBoxButton(
        1,
        _dispatcher,
        GooseUI::SCALE_NONE, 
        GooseUI::ALIGN_TOP | GooseUI::ALIGN_BOTTOM | GooseUI::ALIGN_LEFT | GooseUI::ALIGN_RIGHT,
        20, 20,
        100, 100
    );

    
    _button2 = GooseUI::widgets::createBoxButton(
        2,
        _dispatcher,
        GooseUI::SCALE_NONE, 
        GooseUI::ALIGN_TOP | GooseUI::ALIGN_BOTTOM | GooseUI::ALIGN_LEFT | GooseUI::ALIGN_RIGHT,
        20, 20,
        50, 50
    );
    

    _dispatcher.add(1, [&](GooseUI::event::data evt){
        printf("[Window Evt] [ID: 1] - Input Detected \n");
    });

    
    _dispatcher.add(2, [&](GooseUI::event::data evt){
        printf("[Window Evt] [ID: 2] - Input Detected \n");
    });
    

    _button->addToWindow(_window);
    _button2->addToWindow(_window2);

    _window->show();
    _window2->show();

    GooseUI::event::loop::add(_window);
    GooseUI::event::loop::add(_window2);
    GooseUI::event::loop::run();
}