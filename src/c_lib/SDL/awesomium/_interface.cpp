#include "_interface.hpp"

#if GS_AWESOMIUM

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

class ViewportManager* viewport_manager = NULL;

static bool _login_page_loaded = false;
bool login_page_loaded()
{
    return _login_page_loaded;
}

void update()
{
    awe_webcore_update();
}

void draw()
{
    IF_ASSERT(viewport_manager == NULL) return;
    viewport_manager->update();
    viewport_manager->draw();
}

awe_string* get_awe_string(const char* str)
{
    size_t length = strlen(str);
    return awe_string_create_from_ascii(str, length);
}

char* get_str_from_awe(const awe_string* str)
{
    size_t len = awe_string_get_length(str);
    char* dest = (char*)malloc((len+1)*sizeof(char));
    awe_string_to_utf8(str, dest, len);
    dest[len] = '\0';
    return dest;
}

void init()
{
    printf("Awesomium::init\n");

    #ifdef linux
    # if PRODUCTION
    const char package_path_str[] = "./lib/lin32/awesomium/release";
    # else
    const char package_path_str[] = "../lib/lin32/awesomium/release";
    # endif
    awe_string* package_path = get_awe_string(package_path_str);
    const char locale_path_str[] = "./locales"; // relative to the package path
    awe_string* locale_path = get_awe_string(locale_path_str);
    #else
    const awe_string* package_path = awe_string_empty();
    const awe_string* locale_path = awe_string_empty();
    #endif

    awe_string* _data_path = NULL;
    const awe_string* data_path = NULL;
    if (ClientState::active_system_data_path != NULL)
    {
        _data_path = get_awe_string(ClientState::active_system_data_path);
        data_path = _data_path;
    }
    else
    {
        data_path = awe_string_empty();
    }

    awe_string* log_path = get_awe_string("./screenshot");
    awe_string* user_agent = get_awe_string(USER_AGENT);

    awe_webcore_initialize(
        false, //plugins
        true, //javascript
        false,  //databases
        package_path,   //package path
        locale_path,    //locale path
        data_path, //user data path
        awe_string_empty(), //plugin path
        log_path, // log path
        AWE_LL_VERBOSE, //log level
        false, //force single process
        awe_string_empty(), //child process Path,
        true,   // autodetect encoding
        awe_string_empty(), // accept language override
        awe_string_empty(), // default charset override
        user_agent, // user agent
        awe_string_empty(), // proxy server
        awe_string_empty(), // proxy config script
        awe_string_empty(), // auth server whitelist
        true,   // save cache and cookies
        0,      // max cache size
        true,  // disable same origin policy. we need this disabled to do ajax request from local to webserver
        false,  // disable win message pump
        awe_string_empty()  // custom css
    );

    #ifdef linux
    awe_string_destroy(package_path);
    awe_string_destroy(locale_path);
    #endif

    if (_data_path != NULL)
        awe_string_destroy(_data_path);
    awe_string_destroy(log_path);
    awe_string_destroy(user_agent);

    const char _curdir[] = ".";
    awe_string* curdir = get_awe_string(_curdir);
    awe_webcore_set_base_directory(curdir);
    awe_string_destroy(curdir);

    GS_ASSERT(viewport_manager == NULL);
    viewport_manager = new ViewportManager;
}

void teardown()
{
    delete viewport_manager;
    awe_webcore_shutdown();
}

void delete_all_cookies()
{
    awe_webcore_clear_cookies();
}

void delete_cookie(const char* url, const char* name)
{
    awe_string* _url = get_awe_string(url);
    awe_string* _name = get_awe_string(name);
    awe_webcore_delete_cookie(_url, _name);
    awe_string_destroy(_url);
    awe_string_destroy(_name);
}

void delete_cookie(const char* name)
{
    delete_cookie(GNOMESCROLL_URL, name);
}

void delete_auth_token_cookie()
{
    delete_cookie(Auth::AUTH_TOKEN_LOCAL_COOKIE_NAME);
}

void open_token_page()
{   // call js function that makes request for token against server
    IF_ASSERT(viewport_manager == NULL) return;
    class ChromeViewport* cv = viewport_manager->get_login_window();
    IF_ASSERT(cv == NULL || cv->webView == NULL) return;
    awe_string* get_token_fn = get_awe_string(JS_CB_OPEN_TOKEN_PAGE_NAME);
    awe_jsarray* js_args = awe_jsarray_create(NULL, 0);
    awe_webview_call_javascript_function(cv->webView, awe_string_empty(), get_token_fn, js_args, awe_string_empty());
    awe_string_destroy(get_token_fn);
    awe_jsarray_destroy(js_args);
}

void send_json_settings(class SettingsExport* exporter)
{
    IF_ASSERT(viewport_manager == NULL) return;
    class ChromeViewport* cv = viewport_manager->get_settings_window();
    IF_ASSERT(cv == NULL || cv->webView == NULL) return;
    char* _settings = exporter->export_json_varlist();
    char* _display_elements = exporter->export_json_display_element();
    awe_string* fn = get_awe_string(JS_CB_SEND_JSON_SETTINGS);
    awe_string* settings = get_awe_string(_settings);
    awe_string* display_elements = get_awe_string(_display_elements);
    const int nargs = 2;
    int i = 0;
    awe_jsvalue* values[nargs];
    values[i++] = awe_jsvalue_create_string_value(settings);
    values[i++] = awe_jsvalue_create_string_value(display_elements);
    GS_ASSERT(i == nargs);
    awe_jsarray* args = awe_jsarray_create((const awe_jsvalue**)values, nargs);
    awe_webview_call_javascript_function(cv->webView, awe_string_empty(), fn, args, awe_string_empty());
    awe_string_destroy(fn);
    awe_jsarray_destroy(args);
    awe_string_destroy(settings);
    awe_string_destroy(display_elements);
    for (int i=0; i<nargs; i++)
        awe_jsvalue_destroy(values[i]);
    free(_settings);
    free(_display_elements);
}

void SDL_keyboard_event(const SDL_Event* event)
{
    IF_ASSERT(viewport_manager == NULL) return;
    viewport_manager->handle_keyboard_event(event);
}

void SDL_mouse_event(const SDL_Event* event)
{
    IF_ASSERT(viewport_manager == NULL) return;
    viewport_manager->handle_mouse_event(event);
}

void enable()
{
    IF_ASSERT(viewport_manager == NULL) return;
    viewport_manager->enable();
}

void disable()
{
    IF_ASSERT(viewport_manager == NULL) return;
    viewport_manager->disable();
}

char* get_cookies(const char* _url)
{
    awe_string* url = get_awe_string(_url);
    const awe_string* _cookies = awe_webcore_get_cookies(url, false);
    awe_string_destroy(url);

    char* cookies = get_str_from_awe(_cookies);
    return cookies;
}

char* get_cookies()
{
    return get_cookies(GNOMESCROLL_URL);
}

char* get_auth_token()
{
    char* cookies = get_cookies();
    IF_ASSERT(cookies == NULL) return NULL;

    char* token = strstr(cookies, Auth::AUTH_TOKEN_LOCAL_COOKIE_NAME);
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
    auth_token = strcpy(auth_token, &token[strlen(Auth::AUTH_TOKEN_LOCAL_COOKIE_NAME) + 1]);    // copy cookie value to auth_token. offset is strlen(token_name) + strlen(=)
    free(cookies);
    return auth_token;
}

void check_for_token_cookie(const awe_string* _url)
{   // grab the cookie and send it to the auth system
    char* url = get_str_from_awe(_url);
    if (strstr(url, GNOMESCROLL_URL) != NULL)
    {
        char* token = get_auth_token();
        if (token != NULL)
        {
            Auth::AuthError error = Auth::update_token(token);
            IF_ASSERT(error != Auth::AUTH_ERROR_NONE)
                printf("Auth error code: %d\n", error);
        }
        free(token);
    }
    free(url);
}

char* make_cookie_expiration_string(const time_t expiration_time)
{   // FREE THE RETURNED STRING     //DAY, DD-MMM-YYYY HH:MM:SS GMT
    const static char timefmt[] = "%a %d-%b-%Y %H:%M:%S GMT";
    const static size_t size = sizeof(timefmt) - 2*7 + 3 + 2 + 3 + 4 + 2 + 2 + 2;
    struct tm* tmdata = gmtime(&expiration_time);
    char* expiration_str = (char*)malloc((size+1) * sizeof(char));
    strftime(expiration_str, size, timefmt, tmdata);
    expiration_str[size] = '\0';
    return expiration_str;
}

void set_game_token_cookie(const char* token, time_t expiration_time)
{   // manually set the cookie for gnomescroll game tokens
    static const size_t prefix_len = strlen(Auth::AUTH_TOKEN_LOCAL_COOKIE_NAME);
    static const size_t domain_len = strlen(GNOMESCROLL_COOKIE_DOMAIN);
    const size_t token_len = strlen(token);
    char* expiration_str = make_cookie_expiration_string(expiration_time);
    const size_t expiration_len = strlen(expiration_str);
    const static char cookie_fmt[] = "%s=%s; expires=%s; domain=%s; path=/;";
    char* _cookie = (char*)malloc((sizeof(cookie_fmt) - 2*4 + prefix_len + token_len + domain_len + expiration_len) * sizeof(char));
    sprintf(_cookie, cookie_fmt, Auth::AUTH_TOKEN_LOCAL_COOKIE_NAME, token, expiration_str, GNOMESCROLL_COOKIE_DOMAIN);
    free(expiration_str);

    awe_string* url = get_awe_string(GNOMESCROLL_URL);
    awe_string* cookie = get_awe_string(_cookie);
    free(_cookie);
    awe_webcore_set_cookie(url, cookie, true, false);
    awe_string_destroy(cookie);
    awe_string_destroy(url);

    // REFERENCE:
    //awe_webcore_set_cookie(url, cookie, is_http_only, force_session_cookie -- will be cleared on exit);
}

void save_username(const char* username)
{   // set a these on a cookie for localhost
    time_t expiration_time = utc_now() + (60 * 60 * 24 * 365);  // expires in a year
    char* expiration_str = make_cookie_expiration_string(expiration_time);
    size_t expiration_len = strlen(expiration_str);

    size_t username_len = strlen(username);

    #if PRODUCTION
    const static char cookie_fmt[] = "username=%s; expires=%s; domain=127.0.0.1; path=/;";
    #else
    const static char cookie_fmt[] = "dbgusername=%s; expires=%s; domain=127.0.0.1; path=/;";
    #endif
    size_t cookie_len = strlen(cookie_fmt) - 2*2 + username_len + expiration_len + 1;
    char* _cookie = (char*)malloc((cookie_len+1) * sizeof(char));
    snprintf(_cookie, cookie_len, cookie_fmt, username, expiration_str);
    _cookie[cookie_len] = '\0';
    free(expiration_str);

    awe_string* cookie = get_awe_string(_cookie);

    awe_string* url = get_awe_string("http://127.0.0.1");
    awe_webcore_set_cookie(url, cookie, true, false);
    awe_string_destroy(cookie);
    awe_string_destroy(url);
    free(_cookie);

    // REFERENCE:
    //awe_webcore_set_cookie(url, cookie, is_http_only, force_session_cookie -- will be cleared on exit);
}

void save_password(const char* password)
{   // set a these on a cookie for localhost
    time_t expiration_time = utc_now() + (60 * 60 * 24 * 365);  // expires in a year
    char* expiration_str = make_cookie_expiration_string(expiration_time);
    size_t expiration_len = strlen(expiration_str);

    size_t password_len = strlen(password);

    #if PRODUCTION
    const static char cookie_fmt[] = "password=%s; expires=%s; domain=127.0.0.1; path=/;";
    #else
    const static char cookie_fmt[] = "dbgpassword=%s; expires=%s; domain=127.0.0.1; path=/;";
    #endif
    size_t cookie_len = strlen(cookie_fmt) - 2*2 + password_len + expiration_len + 1;
    char* _cookie = (char*)malloc((cookie_len+1) * sizeof(char));
    snprintf(_cookie, cookie_len, cookie_fmt, password, expiration_str);
    _cookie[cookie_len] = '\0';
    free(expiration_str);

    awe_string* cookie = get_awe_string(_cookie);

    awe_string* url = get_awe_string("http://127.0.0.1");
    awe_webcore_set_cookie(url, cookie, true, false);
    awe_string_destroy(cookie);
    awe_string_destroy(url);
    free(_cookie);

    // REFERENCE:
    //awe_webcore_set_cookie(url, cookie, is_http_only, force_session_cookie -- will be cleared on exit);
}

void save_remember_password_setting(bool _remember)
{   // set a these on a cookie for localhost
    time_t expiration_time = utc_now() + (60 * 60 * 24 * 365);  // expires in a few years
    char* expiration_str = make_cookie_expiration_string(expiration_time);
    size_t expiration_len = strlen(expiration_str);

    int remember = (_remember) ? 1 : 0;

    #if PRODUCTION
    const static char cookie_fmt[] = "remember_password=%d; expires=%s; domain=127.0.0.1; path=/;";
    #else
    const static char cookie_fmt[] = "dbgremember_password=%d; expires=%s; domain=127.0.0.1; path=/;";
    #endif
    size_t cookie_len = strlen(cookie_fmt) - 2*2 + count_digits(remember) + expiration_len + 1;
    char* _cookie = (char*)malloc((cookie_len+1) * sizeof(char));
    snprintf(_cookie, cookie_len, cookie_fmt, remember, expiration_str);
    _cookie[cookie_len] = '\0';
    free(expiration_str);

    awe_string* cookie = get_awe_string(_cookie);

    awe_string* url = get_awe_string("http://127.0.0.1");
    awe_webcore_set_cookie(url, cookie, true, false);
    awe_string_destroy(cookie);
    awe_string_destroy(url);
    free(_cookie);

    // REFERENCE:
    //awe_webcore_set_cookie(url, cookie, is_http_only, force_session_cookie -- will be cleared on exit);
}

void clear_username()
{
    delete_cookie("http://127.0.0.1", "username");
}

void clear_password()
{
    delete_cookie("http://127.0.0.1", "password");
}

void get_credentials(char** _username, char** _password)
{   // MUST FREE THE RESULTS
    *_username = NULL;
    *_password = NULL;

    char* cookies = get_cookies("http://127.0.0.1");

    #if PRODUCTION
    const char username_key[] ="username=";
    const char password_key[] ="password=";
    #else
    const char username_key[] ="dbgusername=";
    const char password_key[] ="dbgpassword=";
    #endif

    // get the positions of the tokens
    char* username = strstr(cookies, username_key);
    char* password = strstr(cookies, password_key);

    // split the cookie up
    int i=0;
    char c;
    while ((c = cookies[i++]) != '\0')
        if (c == ';') cookies[i-1] = '\0';

    // copy the data over
    if (username != NULL)
    {
        username = &username[strlen(username_key)];
        size_t username_len = strlen(username);
        *_username = (char*)malloc((username_len+1)*sizeof(char));
        strcpy(*_username, username);
    }

    if (password != NULL)
    {
        password = &password[strlen(password_key)];
        size_t password_len = strlen(password);
        *_password = (char*)malloc((password_len+1)*sizeof(char));
        strcpy(*_password, password);
    }

    free(cookies);
}

bool get_remember_password_setting()
{
    char* cookies = get_cookies("http://127.0.0.1");

    #if PRODUCTION
    const char remember_pass_key[] = "remember_password=";
    #else
    const char remember_pass_key[] = "dbgremember_password=";
    #endif

    // get position inside cookie
    char* _remember = strstr(cookies, remember_pass_key);
    // split the cookie up
    int i=0;
    char c;
    while ((c = cookies[i++]) != '\0')
        if (c == ';') cookies[i-1] = '\0';

    bool remember = false;
    if (_remember != NULL)
        remember = (bool)atoi(&_remember[strlen(remember_pass_key)]);
    free(cookies);
    return remember;
}

}   // Awesomium

#endif
