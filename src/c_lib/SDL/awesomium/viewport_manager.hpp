#pragma once

#if GS_AWESOMIUM

#include <SDL/awesomium/viewport_class.hpp>

namespace Awesomium
{

const int MAX_VIEWPORTS = 8;;

class ViewportManager
{
    public:

        bool in_focus; //is one of the windows in focus

        class ChromeViewport* vpa[MAX_VIEWPORTS]; //viewport class array
        int vpai; //viewport class array index

    ViewportManager() :
    in_focus(false)
    {
        for(int i=0; i<MAX_VIEWPORTS; vpa[i++] = NULL);
    }

    void add_viewport(class ChromeViewport* viewport)
    {
        for(int i=0; i < MAX_VIEWPORTS; i++)
        {
            if(vpa[i] == NULL)
            {
                vpa[i] = viewport;
                printf("Attached viewport to slot %i \n", i);
                return;
            }
        }

        printf("Error: viewport_manager, add_viewport; max viewports reached \n");
    }

    void update_viewports();
    void draw_viewports();

    void handle_mouse_event(int x, int y, int button, int event_type) //up/down
    {
        if(event_type == 0)
            printf("Mouse %i down at: %i %i \n", button,x,y);
        if(event_type == 1)
            printf("Mouse %i down at: %i %i \n", button,x,y);

        for(int i=0; i<MAX_VIEWPORTS; i++)
        {
            if(vpa[i] == NULL) continue;


            //handle window focusing

            //if a click hits a window, determine if the window is in focus
            //if window is in focus, then pass click on
            //if windows is not in focus, then focus
        }
    }

    void handle_keyboard_event(union SDL_Event* keyEvent)
    {
        //if a window is in focus
    }

};

//updates and draws
void ViewportManager::draw_viewports()
{
    for(int i=0; i < MAX_VIEWPORTS; i++)
        if(vpa[i] != NULL)
        {
            vpa[i]->update_webview();
            vpa[i]->draw_webview();
        }
}

// Awesomium::LEFT_MOUSE_BTN
// Awesomium::MIDDLE_MOUSE_BTN
// Awesomium::RIGHT_MOUSE_BTN

// mouse down
// myWebView->injectMouseDown(Awesomium::LEFT_MOUSE_BTN);

// mouse up
// myWebView->injectMouseUp(Awesomium::LEFT_MOUSE_BTN);

// mouse scroll
// myWebView->injectMouseWheel(100);

}   // Awesomium

#endif
