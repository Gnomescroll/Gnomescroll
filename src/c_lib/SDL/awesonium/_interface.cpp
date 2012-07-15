
#include "_interface.hpp"


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


    awe_string* get_awe_string(const char* _str)
    {
        int length = strlen(_str);
        return awe_string_create_from_ascii(_str, length);
    }

    void init()
    {
        //awe_webcore_initialize_default();
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
    //libraries/lin32/awesomium/release/locales/
    awe_string* package_path = get_awe_string("../libraries/lin32/awesomium/release");
    awe_string* locale_path = get_awe_string("../libraries/lin32/awesomium/release/locales");


    //awe_webview_load_html(webView, html_str,awe_string_empty());

    awe_webcore_initialize( false, //plugins
        true, //javascript
        false,  //databases
        package_path,   //package path
        locale_path,    //local path
        awe_string_empty(), //user data path
        awe_string_empty(), //plugin path
        awe_string_empty(), // log path
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

    }

    void teardown()
    {


    }

    void update()
    {
        awe_webcore_update();
    }

//#include <Awesomium/awesomium_capi.h>

// A helper macro, used in 'getWebKeyFromSDLKey'
#define mapKey(a, b) case SDLK_##a: return Awesomium::KeyCodes::AK_##b;

// Translates an SDLKey virtual key code to an Awesomium key code
int getWebKeyFromSDLKey(SDLKey key)
{
    switch(key)
    {
    mapKey(BACKSPACE, BACK)
    mapKey(TAB, TAB)
    mapKey(CLEAR, CLEAR)
    mapKey(RETURN, RETURN)
    mapKey(PAUSE, PAUSE)
    mapKey(ESCAPE, ESCAPE)
    mapKey(SPACE, SPACE)
    mapKey(EXCLAIM, 1)
    mapKey(QUOTEDBL, 2)
    mapKey(HASH, 3)
    mapKey(DOLLAR, 4)
    mapKey(AMPERSAND, 7)
    mapKey(QUOTE, OEM_7)
    mapKey(LEFTPAREN, 9)
    mapKey(RIGHTPAREN, 0)
    mapKey(ASTERISK, 8)
    mapKey(PLUS, OEM_PLUS)
    mapKey(COMMA, OEM_COMMA)
    mapKey(MINUS, OEM_MINUS)
    mapKey(PERIOD, OEM_PERIOD)
    mapKey(SLASH, OEM_2)
    mapKey(0, 0)
    mapKey(1, 1)
    mapKey(2, 2)
    mapKey(3, 3)
    mapKey(4, 4)
    mapKey(5, 5)
    mapKey(6, 6)
    mapKey(7, 7)
    mapKey(8, 8)
    mapKey(9, 9)
    mapKey(COLON, OEM_1)
    mapKey(SEMICOLON, OEM_1)
    mapKey(LESS, OEM_COMMA)
    mapKey(EQUALS, OEM_PLUS)
    mapKey(GREATER, OEM_PERIOD)
    mapKey(QUESTION, OEM_2)
    mapKey(AT, 2)
    mapKey(LEFTBRACKET, OEM_4)
    mapKey(BACKSLASH, OEM_5)
    mapKey(RIGHTBRACKET, OEM_6)
    mapKey(CARET, 6)
    mapKey(UNDERSCORE, OEM_MINUS)
    mapKey(BACKQUOTE, OEM_3)
    mapKey(a, A)
    mapKey(b, B)
    mapKey(c, C)
    mapKey(d, D)
    mapKey(e, E)
    mapKey(f, F)
    mapKey(g, G)
    mapKey(h, H)
    mapKey(i, I)
    mapKey(j, J)
    mapKey(k, K)
    mapKey(l, L)
    mapKey(m, M)
    mapKey(n, N)
    mapKey(o, O)
    mapKey(p, P)
    mapKey(q, Q)
    mapKey(r, R)
    mapKey(s, S)
    mapKey(t, T)
    mapKey(u, U)
    mapKey(v, V)
    mapKey(w, W)
    mapKey(x, X)
    mapKey(y, Y)
    mapKey(z, Z)
    mapKey(DELETE, DELETE)
    mapKey(KP0, NUMPAD0)
    mapKey(KP1, NUMPAD1)
    mapKey(KP2, NUMPAD2)
    mapKey(KP3, NUMPAD3)
    mapKey(KP4, NUMPAD4)
    mapKey(KP5, NUMPAD5)
    mapKey(KP6, NUMPAD6)
    mapKey(KP7, NUMPAD7)
    mapKey(KP8, NUMPAD8)
    mapKey(KP9, NUMPAD9)
    mapKey(KP_PERIOD, DECIMAL)
    mapKey(KP_DIVIDE, DIVIDE)
    mapKey(KP_MULTIPLY, MULTIPLY)
    mapKey(KP_MINUS, SUBTRACT)
    mapKey(KP_PLUS, ADD)
    mapKey(KP_ENTER, SEPARATOR)
    mapKey(KP_EQUALS, UNKNOWN)
    mapKey(UP, UP)
    mapKey(DOWN, DOWN)
    mapKey(RIGHT, RIGHT)
    mapKey(LEFT, LEFT)
    mapKey(INSERT, INSERT)
    mapKey(HOME, HOME)
    mapKey(END, END)
    mapKey(PAGEUP, PRIOR)
    mapKey(PAGEDOWN, NEXT)
    mapKey(F1, F1)
    mapKey(F2, F2)
    mapKey(F3, F3)
    mapKey(F4, F4)
    mapKey(F5, F5)
    mapKey(F6, F6)
    mapKey(F7, F7)
    mapKey(F8, F8)
    mapKey(F9, F9)
    mapKey(F10, F10)
    mapKey(F11, F11)
    mapKey(F12, F12)
    mapKey(F13, F13)
    mapKey(F14, F14)
    mapKey(F15, F15)
    mapKey(NUMLOCK, NUMLOCK)
    mapKey(CAPSLOCK, CAPITAL)
    mapKey(SCROLLOCK, SCROLL)
    mapKey(RSHIFT, RSHIFT)
    mapKey(LSHIFT, LSHIFT)
    mapKey(RCTRL, RCONTROL)
    mapKey(LCTRL, LCONTROL)
    mapKey(RALT, RMENU)
    mapKey(LALT, LMENU)
    mapKey(RMETA, LWIN)
    mapKey(LMETA, RWIN)
    mapKey(LSUPER, LWIN)
    mapKey(RSUPER, RWIN)
    mapKey(MODE, MODECHANGE)
    mapKey(COMPOSE, ACCEPT)
    mapKey(HELP, HELP)
    mapKey(PRINT, SNAPSHOT)
    mapKey(SYSREQ, EXECUTE)
    default: return Awesomium::KeyCodes::AK_UNKNOWN;
    }
}


}