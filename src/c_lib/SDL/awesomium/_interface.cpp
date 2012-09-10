#include "_interface.hpp"

#include <SDL/awesomium/viewport_class.hpp>
#include <SDL/awesomium/viewport_manager.hpp>

#include <auth/constants.hpp>

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

char* get_str_from_awe(const awe_string* str)
{
    size_t len = awe_string_get_length(str);
    char* dest = (char*)malloc((len+1)*sizeof(char));
    awe_string_to_utf8(str, dest, len);
    dest[len] = '\0';
    return dest;
}

bool url_is_whitelisted(const char* url)
{
    if (str_starts_with(url, "file://"))
        return true;
    if (str_starts_with(url, "http://127.0.0.1"))
        return true;
    if (str_starts_with(url, "https://127.0.0.1"))
        return true;
    if (str_starts_with(url, "http://gnomescroll.com"))
        return true;
    if (str_starts_with(url, "https://gnomescroll.com"))
        return true;
    if (str_starts_with(url, "http://www.gnomescroll.com"))
        return true;
    if (str_starts_with(url, "https://www.gnomescroll.com"))
        return true;
    if (str_starts_with(url, "http://wiki.gnomescroll.com"))
        return true;
    if (str_starts_with(url, "https://wiki.gnomescroll.com"))
        return true;
    if (str_starts_with(url, "http://blog.gnomescroll.com"))
        return true;
    if (str_starts_with(url, "https://blog.gnomescroll.com"))
        return true;
    if (strcmp(url, "http://ajax.googleapis.com/ajax/libs/jquery/1.7.1/jquery.min.js") == 0)
        return true;
    if (strcmp(url, "http://www.google-analytics.com/ga.js") == 0)
        return true;
    return false;
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

    awe_string* user_agent = get_awe_string(USER_AGENT);

    awe_webcore_initialize(
        false, //plugins
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
        //awe_string_empty(), // user agent
        user_agent, // user agent
        awe_string_empty(),
        awe_string_empty(),
        awe_string_empty(),
        true,
        0,
        true,  // disable same origin policy. we need this disable to do ajax request from local to webserver
        false,
        awe_string_empty()
    );

    awe_string_destroy(package_path);
    awe_string_destroy(locale_path);
    awe_string_destroy(log_path);
    awe_string_destroy(user_agent);
    #endif

    const char _curdir[] = ".";
    awe_string* curdir = get_awe_string(_curdir);
    awe_webcore_set_base_directory(curdir);
    awe_string_destroy(curdir);

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

void delete_all_cookies()
{
    awe_webcore_clear_cookies();
}

void delete_cookie(const char* name)
{
    awe_string* _url = get_awe_string(GNOMESCROLL_URL);
    awe_string* _name = get_awe_string(name);
    awe_webcore_delete_cookie(_url, _name);
    awe_string_destroy(_url);
    awe_string_destroy(_name);
}

void delete_auth_token_cookie()
{
    delete_cookie(Auth::AUTH_TOKEN_COOKIE_NAME);
}

void open_url(const char* url)
{
    GS_ASSERT_LIMIT(cv != NULL, 1);
    if (cv == NULL) return;
    cv->load_url(url);
}

void open_file(const char* file)
{
    GS_ASSERT_LIMIT(cv != NULL, 1);
    if (cv == NULL) return;
    cv->load_file(file);
}

void open_token_page()
{
    open_url(GNOMESCROLL_URL GNOMESCROLL_TOKEN_PATH);
}

void open_login_page()
{
    open_file(GNOMESCROLL_LOGIN_HTML);
}

void SDL_keyboard_event(const SDL_Event* event)
{
    GS_ASSERT_LIMIT(cv != NULL, 1);
    if (cv == NULL) return;
    injectSDLKeyEvent(cv->webView, event);

    SDLKey key = event->key.keysym.sym;

    // Separate handling for history navigation -- awesomium does not do this by default
    if (event->type == SDL_KEYDOWN)
    {
        if(event->key.keysym.mod & (KMOD_LALT|KMOD_RALT))
        {
            if (key == SDLK_LEFT)
                awe_webview_go_to_history_offset(cv->webView, -1);
            if (key == SDLK_RIGHT)
                awe_webview_go_to_history_offset(cv->webView, 1);
        }

        if (key == SDLK_MINUS)
        {
            char* token = get_auth_token();
            if (token == NULL)
                printf("No token found\n");
            else
            {
                printf("Token: %s\n", token);
                free(token);
            }
        }
    }
}

void SDL_mouse_event(const SDL_Event* event)
{
    GS_ASSERT_LIMIT(cv != NULL, 1);
    if (cv == NULL) return;
    injectSDLMouseEvent(cv->webView, event);
}

void enable()
{
    if (cv == NULL) return;
    cv->focus();
}

void disable()
{
    if (cv == NULL) return;
    cv->unfocus();
}

char* get_cookies()
{
    GS_ASSERT(cv != NULL);
    if (cv == NULL) return NULL;

    awe_string* url = get_awe_string(GNOMESCROLL_URL);
    const awe_string* _cookies = awe_webcore_get_cookies(url, false);
    awe_string_destroy(url);

    char* cookies = get_str_from_awe(_cookies);
    return cookies;
}

char* get_auth_token()
{
    char* cookies = get_cookies();
    GS_ASSERT(cookies != NULL);
    if (cookies == NULL) return NULL;

    char* token = strstr(cookies, Auth::AUTH_TOKEN_COOKIE_NAME);
    if (token == NULL) return NULL; 

    int i=0;
    char c;
    while ((c = token[i++]) != '\0')
        if (c == ';')
        {
            token[i-1] = '\0';
            break;
        }

    size_t len = strlen(token);
    char* auth_token = (char*)malloc((len+1) * sizeof(char));
    auth_token = strcpy(auth_token, &token[strlen(Auth::AUTH_TOKEN_COOKIE_NAME) + 1]);    // copy cookie value to auth_token. offset is strlen(token_name) + strlen(=)
    free(cookies);
    return auth_token;
}

void check_for_token_cookie(const awe_string* _url)
{   // grab the cookie and send it to the auth system
    char* url = get_str_from_awe(_url);
    if (strstr(url, GNOMESCROLL_URL) != NULL)
    {
        char* token = get_auth_token();
        if (token == NULL) return;
        Auth::AuthError error = Auth::update_token(token);
        GS_ASSERT(error == Auth::AUTH_ERROR_NONE);
    }
    free(url);
}

}   // Awesomium
