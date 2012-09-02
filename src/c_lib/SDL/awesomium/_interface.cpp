
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

void _draw()
{
    static int init = 0;
    if(init == 0)
    {
        //init
        cv = new ChromeViewport;
        viewport_manager = new ViewportManager;
        viewport_manager->add_viewport(cv);
        init = 1;
    }
    
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
    awe_string_empty(),
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

    /*
    awe_webcore_initialize  (   bool    enable_plugins,
    bool    enable_javascript,
    bool    enable_databases,
    const awe_string *  package_path,
    const awe_string *  locale_path,
    const awe_string *  user_data_path,
    const awe_string *  plugin_path,
    const awe_string *  log_path,
    awe_loglevel    log_level,
    bool    force_single_process,
    const awe_string *  child_process_path,
    bool    enable_auto_detect_encoding,
    const awe_string *  accept_language_override,
    const awe_string *  default_charset_override,
    const awe_string *  user_agent_override,
    const awe_string *  proxy_server,
    const awe_string *  proxy_config_script,
    const awe_string *  auth_server_whitelist,
    bool    save_cache_and_cookies,
    int     max_cache_size,
    bool    disable_same_origin_policy,
    bool    disable_win_message_pump,
    const awe_string *  custom_css 
    )       
    */

    /*
     enable_plugins              = false
     enable_javascript           = true
     enable_databases            = false
     package_path                = awe_string_empty()
     locale_path                 = awe_string_empty()
     user_data_path              = awe_string_empty()
     plugin_path                 = awe_string_empty()
     log_path                    = awe_string_empty()
     log_level                   = AWE_LL_NORMAL
     forceSingleProcess          = false
     childProcessPath            = (empty)  // awe_string_empty()?
     enable_auto_detect_encoding = true
     accept_language_override    = awe_string_empty()
     default_charset_override    = awe_string_empty()
     user_agent_override         = awe_string_empty()
     proxy_server                = awe_string_empty()
     proxy_config_script         = awe_string_empty()
    const awe_string *  auth_server_whitelist,

     save_cache_and_cookies      = true
     max_cache_size              = 0
     disable_same_origin_policy  = false
     disable_win_message_pump    = false
     custom_css                  = awe_string_empty()
    */
    

}

void teardown()
{
    awe_webcore_shutdown();
}

//#include <Awesomium/awesomium_capi.h>



}
