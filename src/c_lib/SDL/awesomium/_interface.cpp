
#include "_interface.hpp"

#include <SDL/awesomium/viewport_class.hpp>
#include <SDL/awesomium/viewport_manager.hpp>

/*
ChildProcessPath

Indicates the path to the child-process that we will use to render WebViews and plugins (this will be AwesomiumProcess if you leave this empty)

5. Improved Handling of Config Paths
Rewrote the way we handle config paths, now embedders can specify a “packagePath” that will be used to resolve all relative config paths. 
Embedders can now also specify a “localePath” for the location of en-US.dll on Windows and chrome.pak on Linux.

Rewrote the way we handle config paths, now embedders can specify a “packagePath” that will be used to resolve all relative config paths. 
Embedders can now also specify a “localePath” for the location of en-US.dll on Windows and chrome.pak on Linux.
*/

namespace Awesomium
{

class ChromeViewport* cv = NULL;
class ViewportManager* viewport_manager = NULL;


void handle_mouse_event(int x, int y, int button, int event_type)
{
    viewport_manager->handle_mouse_event(x,y,button,event_type);
}

void handle_keyboard_event(union SDL_Event* keyEvent)
{
    viewport_manager->handle_keyboard_event(keyEvent);
}

void update()
{
    awe_webcore_update();
}

void draw()
{
    GS_ASSERT_LIMIT(cv != NULL, 1);
    if (cv == NULL) return;
    cv->update_webview();
    cv->draw_webview();
}

awe_string* get_awe_string(const char* _str)
{
    int length = strlen(_str);
    return awe_string_create_from_ascii(_str, length);
}

void init()
{
    printf("Awesomium::init\n");
    
    #ifndef linux
    // TODO -- non default initializer
    awe_webcore_initialize_default();
    #endif
        
    #ifdef linux

    #if PRODUCTION
    const char package_path_str[] = "./lib/lin32/awesomium/release";
    const char locale_path_str[] = "./lib/lin32/awesomium/release/locales";
    #else
    const char package_path_str[] = "../lib/lin32/awesomium/release";
    const char locale_path_str[] = "../lib/lin32/awesomium/release/locales";
    #endif
    
    awe_string* package_path = get_awe_string(package_path_str);
    awe_string* locale_path = get_awe_string(locale_path_str);
    awe_string* log_path = get_awe_string("./screenshot");

    //user_agent_fmt = "Gnomescroll/%d";
    //awe_string* user_agent = get_awe_string("Gnomescroll/%d

    //awe_webview_load_html(webView, html_str,awe_string_empty());

    awe_webcore_initialize( false, //plugins
    true, //javascript
    false,  //databases
    package_path,   //package path
    locale_path,    //local path
    awe_string_empty(), //user data path
    awe_string_empty(), //plugin path
    log_path, // log path
    AWE_LL_NORMAL, //log level
    false, //force single process
    awe_string_empty(), //child process Path,
    true,
    awe_string_empty(),
    awe_string_empty(),
    awe_string_empty(), // user agent
    awe_string_empty(),
    awe_string_empty(),
    awe_string_empty(),
    true,
    0,
    false, 
    false,
    awe_string_empty()
    );

    awe_string_destroy(package_path);
    awe_string_destroy(locale_path);
    awe_string_destroy(log_path);
    #endif


    GS_ASSERT(cv == NULL);
    GS_ASSERT(viewport_manager == NULL);

    cv = new ChromeViewport;
    viewport_manager = new ViewportManager;
    viewport_manager->add_viewport(cv);
}

void teardown()
{
    if (cv != NULL) delete cv; 
    if (viewport_manager != NULL) delete viewport_manager; 
    awe_webcore_shutdown();
}

}   // Awesomium
