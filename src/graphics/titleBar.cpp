#include "GooseUI/graphics/titleBar.h"
#include "GooseUI/abstractions/iWindow.h"


namespace GooseUI::graphics // Public
{
    void titleBar::createDefaultDecorations(windowDecoration type, titleBarData *&titleBar, absractions::iWindow *window, int windowEventID, event::dispatcher &evtDispatcher)
    {
        // We will add the events in the window it self
        titleBar = new graphics::titleBarData();
        titleBar->evtDispatcher = &evtDispatcher;

        // Header - windowEventID
        titleBar->bar = new widgets::titleBar(windowEventID, evtDispatcher, SCALE_HORIZONTAL, ALIGN_LEFT | ALIGN_RIGHT | ALIGN_TOP, 0, 0, window->getWidth(), DEF_GSA_WINDOW_TITLEBAR_HEIGHT);
        titleBar->bar->addToWindow(window);

        // Close Button - windowEventID + 1
        {
            boxButtonCreationInfo info {};
            info.eventID = windowEventID + 1;
            info.evtDispatcher = &evtDispatcher;
            info.scaleing = SCALE_NONE; 
            info.alignment = ALIGN_RIGHT | ALIGN_TOP | ALIGN_BOTTOM;
            info.X = titleBar->bar->getWidth() - DEF_GSA_WINDOW_TITLEBAR_HEIGHT;
            info.Y = titleBar->bar->getHeight() - DEF_GSA_WINDOW_TITLEBAR_HEIGHT + 2;
            info.width = DEF_GSA_WINDOW_TITLEBAR_HEIGHT - 6;
            info.height = DEF_GSA_WINDOW_TITLEBAR_HEIGHT - 6;
            titleBar->closeButton = widgets::createBoxButton(info);
            
            titleBar->closeButton->setColor({ 0.91f, 0.12f, 0.15f, 1.0f });
            titleBar->closeButton->setParent(titleBar->bar);
        }
    }

    void titleBar::removeDefaultDecorations(titleBarData *titleBar, int windowEventID, absractions::iWindow *window)
    {
        if(titleBar != nullptr)
        {
            if(titleBar->closeButton != nullptr)
            { 
                titleBar->closeButton->removeParent(); 
                titleBar->evtDispatcher->remove(windowEventID + 1);
            }

            if(titleBar->bar != nullptr)
            {
                titleBar->bar->removeFromWindow();
                titleBar->evtDispatcher->remove(windowEventID);
            }

            delete(titleBar);
            titleBar = nullptr;
        }
    }

    void titleBar::modifieDefaultDecorations(){}
}