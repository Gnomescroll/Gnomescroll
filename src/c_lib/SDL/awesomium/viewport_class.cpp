#include "viewport_class.hpp"

#if GS_AWESOMIUM

#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#include <hud/hud.hpp>
#include <chat/_interface.hpp>

namespace Awesomium
{

/*
struct chromeDisplay {
    int x;
    int y;
    int width;
    int height;
    int tex_id;
    void* webView;
};
*/

/*
    Begin navigation callback triggered
    URL on webView is: http://127.0.0.1:5002/server/token
    Frame name is:

    Begin loading callback triggered
    URL on webView is: http://127.0.0.1:5002/server/list
    Frame name is:
    Status code is; 200
    Mime type is: text/html

    -------
    Track between these two calls to detect the redirect for token page
    (could not get redirect response measured in any callback)
*/

void js_console_message_callback(awe_webview *webView, const awe_string *_message, int line_number, const awe_string *_source)
{
    char* source = get_str_from_awe(_source);
    char* message = get_str_from_awe(_message);
    printf("console.log: %s:%d\n", source, line_number);
    printf("%s\n", message);
    free(message);
    free(source);
}

void begin_navigation_cb(awe_webview* webView, const awe_string* _url, const awe_string* _frame_name)
{
    // this callback triggers after a url is opened with open_url
    // and after the header for the response has been received
}

void begin_loading_cb(awe_webview* webView, const awe_string* _url, const awe_string* _frame_name, int status_code, const awe_string* _mime_type)
{
    //printf("Begin loading callback triggered\n");
    //char* url = get_str_from_awe(_url);
    //char* mime_type = get_str_from_awe(_mime_type);
    //printf("URL on webView is: %s\n", url);
    //printf("Status code is; %d\n", status_code);
    //printf("Mime type is: %s\n", mime_type);
    //free(url);
    //free(mime_type);

    //if (strstr(url, "/server/list") != NULL)
    //{
        //char* cookies = get_cookies();
        //printf("Cookies: %s\n", cookies);
        //free(cookies);
    //}

    //printf("\n");

    //check_for_token_cookie(_url);
}

void finish_loading_cb(awe_webview* webView)
{
    awe_string* _url = awe_webview_get_url(webView);
    char* url = get_str_from_awe(_url);
    awe_string_destroy(_url);

    if (str_ends_with(url, GNOMESCROLL_LOGIN_HTML))
        _login_page_loaded = true;

    free(url);
}

awe_resource_response* resource_request_cb(awe_webview* webView, awe_resource_request* request)
{
    return NULL;
}

void resource_response_cb(awe_webview* webView, const awe_string* _url,
    int status_code, bool was_cached,
    int64 request_time_ms, int64 response_time_ms, int64 expected_content_size,
    const awe_string *_mime_type)
{
    printf("Resource response callback triggered\n");
    char* url = get_str_from_awe(_url);
    printf("URL: %s\n", url);
    printf("Status code: %d\n", status_code);
    free(url);
    printf("\n");
}

void web_view_crashed_cb(awe_webview* webView)
{
    printf("Webview crashed\n");
    viewport_manager->webview_crash(webView);
}

void js_set_message_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    const awe_jsvalue* vmsg = awe_jsarray_get_element(_args, 0);
    IF_ASSERT(vmsg == NULL) return;

    awe_string* _msg = awe_jsvalue_to_string(vmsg);
    char* msg = get_str_from_awe(_msg);
    Hud::set_awesomium_message(msg);
    awe_string_destroy(_msg);
    free(msg);
}

void js_unset_message_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    Hud::clear_awesomium_message();
}

void js_set_token_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    const awe_jsvalue* vtoken = awe_jsarray_get_element(_args, 0);
    IF_ASSERT(vtoken == NULL) return;

    awe_string* _token = awe_jsvalue_to_string(vtoken);
    char* token = get_str_from_awe(_token);
    Auth::AuthError error = Auth::update_token(token);
    if (error != Auth::AUTH_ERROR_NONE)
        printf("Auth error code: %d\n", error);
    IF_ASSERT(error != Auth::AUTH_ERROR_NONE)
    {
        Hud::set_awesomium_message("Authentication server failure. Try again soon.");
        Auth::token_failure = true;
    }
    free(token);
    awe_string_destroy(_token);
}

void js_token_failure_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    Auth::token_failure = true;
}

void js_login_required_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    // Required token refreshing is disabled for now

    //Auth::needs_login = true;
    //Chat::send_system_message("There was a server reset. You will need to again soon to continue playing.");
    //Chat::send_system_message(Hud::open_login_text);
    if (!Auth::has_authorized_once)
        enable_login_window();
    //else
        //Hud::set_prompt(Hud::open_login_text);
}

void js_save_username_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    const awe_jsvalue* vusername = awe_jsarray_get_element(_args, 0);
    if (vusername == NULL) return;
    if (awe_jsvalue_get_type(vusername) != JSVALUE_TYPE_STRING) return;

    awe_string* _username = awe_jsvalue_to_string(vusername);
    char* username = get_str_from_awe(_username);
    if (username[0] == '\0')
        clear_username();
    else
        save_username(username);
    awe_string_destroy(_username);
    free(username);
}

void js_save_password_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    const awe_jsvalue* vpassword = awe_jsarray_get_element(_args, 0);
    if (vpassword == NULL) return;
    if (awe_jsvalue_get_type(vpassword) != JSVALUE_TYPE_STRING) return;

    awe_string* _password = awe_jsvalue_to_string(vpassword);
    char* password = get_str_from_awe(_password);
    if (password[0] == '\0')
        clear_password();
    else
        save_password(password);
    awe_string_destroy(_password);
    free(password);
}

void js_save_remember_password_settings_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    const awe_jsvalue* vremember = awe_jsarray_get_element(_args, 0);
    if (vremember == NULL) return;

    bool remember = awe_jsvalue_to_boolean(vremember);
    save_remember_password_setting(remember);
}

void js_change_setting_value(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    using Options::settings;
    const awe_jsvalue* vname = awe_jsarray_get_element(_args, 0);
    IF_ASSERT(vname == NULL) return;
    awe_string* _name = awe_jsvalue_to_string(vname);
    char* name = get_str_from_awe(_name);
    awe_string_destroy(_name);

    const awe_jsvalue* vval = awe_jsarray_get_element(_args, 1);
    awe_jsvalue_type vtype = awe_jsvalue_get_type(vval);
    ConfigType type = settings->get_config_type(name);
    IF_ASSERT(vtype == JSVALUE_TYPE_NULL || vtype == JSVALUE_TYPE_ARRAY ||
              vtype == JSVALUE_TYPE_OBJECT)
        goto cleanup;
    IF_ASSERT(type == CONFIG_TYPE_NONE)
        goto cleanup;
    IF_ASSERT(type == CONFIG_TYPE_BOOL && vtype != JSVALUE_TYPE_BOOLEAN)
        goto cleanup;
    IF_ASSERT(type == CONFIG_TYPE_STRING && vtype != JSVALUE_TYPE_STRING)
        goto cleanup;
    IF_ASSERT((type == CONFIG_TYPE_FLOAT || type == CONFIG_TYPE_INT) &&
              (vtype != JSVALUE_TYPE_INTEGER && vtype != JSVALUE_TYPE_DOUBLE))
        goto cleanup;

    printf("Setting %s to new value\n", name);

    if (vtype == JSVALUE_TYPE_BOOLEAN)
    {
        bool val = awe_jsvalue_to_boolean(vval);
        settings->set(name, val);
    }
    else
    if (vtype == JSVALUE_TYPE_STRING)
    {
        awe_string* _val = awe_jsvalue_to_string(vval);
        char* val = get_str_from_awe(_val);
        awe_string_destroy(_val);
        settings->set(name, val);
        free(val);
    }
    else
    if (vtype == JSVALUE_TYPE_INTEGER)
    {
        int val = awe_jsvalue_to_integer(vval);
        settings->set(name, val);
    }
    else
    if (vtype == JSVALUE_TYPE_DOUBLE)
    {
        float val = awe_jsvalue_to_double(vval);
        settings->set(name, val);
    }
    else
    if (vtype == JSVALUE_TYPE_ARRAY)
    {
        Color color = Color(0);
        const awe_jsarray* arr = awe_jsvalue_get_array(vval);
        size_t len = awe_jsarray_get_size(arr);
        GS_ASSERT(len == 3);
        if (len == 3)
        {
            bool ok = true;
            for (size_t i=0; i<3; i++)
            {
                const awe_jsvalue* _c = awe_jsarray_get_element(arr, i);
                IF_ASSERT(awe_jsvalue_get_type(_c) != JSVALUE_TYPE_INTEGER)
                {
                    ok = false;
                    break;
                }
                int c = awe_jsvalue_to_integer(_c);
                IF_ASSERT(c < 0 || c > 0xFF)
                {
                    ok = false;
                    break;
                }
                color.c[i] = c;
            }
            if (ok)
                settings->set(name, color);
        }
    }

    cleanup:
        free(name);
}

void js_callback_handler(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    char* cb = get_str_from_awe(_cb_name);
    if (strcmp(cb, JS_CB_SET_MESSAGE_NAME) == 0)
        js_set_message_callback(webView, _obj_name, _cb_name, _args);
    else
    if (strcmp(cb, JS_CB_UNSET_MESSAGE_NAME) == 0)
        js_unset_message_callback(webView, _obj_name, _cb_name, _args);
    else
    if (strcmp(cb, JS_CB_SET_TOKEN_NAME) == 0)
        js_set_token_callback(webView, _obj_name, _cb_name, _args);
    else
    if (strcmp(cb, JS_CB_TOKEN_FAILURE_NAME) == 0)
        js_token_failure_callback(webView, _obj_name, _cb_name, _args);
    else
    if (strcmp(cb, JS_CB_LOGIN_REQUIRED_NAME) == 0)
        js_login_required_callback(webView, _obj_name, _cb_name, _args);
    else
    if (strcmp(cb, JS_CB_SAVE_USERNAME_NAME) == 0)
        js_save_username_callback(webView, _obj_name, _cb_name, _args);
    else
    if (strcmp(cb, JS_CB_SAVE_PASSWORD_NAME) == 0)
        js_save_password_callback(webView, _obj_name, _cb_name, _args);
    else
    if (strcmp(cb, JS_CB_SAVE_REMEMBER_PASSWORD_SETTING_NAME) == 0)
        js_save_remember_password_settings_callback(webView, _obj_name, _cb_name, _args);
    if (strcmp(cb, JS_CB_CHANGE_SETTING_VALUE) == 0)
        js_change_setting_value(webView, _obj_name, _cb_name, _args);
    else
        printf("Unhandled javascript callback triggered: %s\n", cb);
    free(cb);
}

void change_keyboard_focus_cb(awe_webview* webView, bool is_focused)
{
    //printf("Change keyboard focus callback: ");
    //if (is_focused) printf("is focused\n");
    //else printf("not focused\n");
}

void change_cursor_cb(awe_webview* webView, awe_cursor_type cursor)
{
    //printf("Change cursor callback");
}

void ChromeViewport::set_callbacks()
{
    //awe_webview_set_callback_begin_navigation(this->webView, &begin_navigation_cb);
    //awe_webview_set_callback_begin_loading(this->webView, &begin_loading_cb);
    awe_webview_set_callback_finish_loading(this->webView, &finish_loading_cb);
    //awe_webview_set_callback_resource_response(this->webView, &resource_response_cb);
    //awe_webview_set_callback_resource_request(this->webView, &resource_request_cb);
    awe_webview_set_callback_web_view_crashed(this->webView, &web_view_crashed_cb);
    //awe_webview_set_callback_change_keyboard_focus(this->webView, &change_keyboard_focus_cb);
    //awe_webview_set_callback_change_cursor(this->webView, &change_cursor_cb);
}

// A helper macro, used in 'get_web_key_from_SDL_key'
//#define MAP_KEY(a, b) case SDLK_##a: return Awesomium::KeyCodes::AK_##b;
#define MAP_KEY(a, b) case SDLK_##a: return AK_##b;

// Translates an SDLKey virtual key code to an Awesomium key code
int get_web_key_from_SDL_key(SDLKey key)
{
    switch (key)
    {
    MAP_KEY(BACKSPACE, BACK)
    MAP_KEY(TAB, TAB)
    MAP_KEY(CLEAR, CLEAR)
    MAP_KEY(RETURN, RETURN)
    MAP_KEY(PAUSE, PAUSE)
    MAP_KEY(ESCAPE, ESCAPE)
    MAP_KEY(SPACE, SPACE)
    MAP_KEY(EXCLAIM, 1)
    MAP_KEY(QUOTEDBL, 2)
    MAP_KEY(HASH, 3)
    MAP_KEY(DOLLAR, 4)
    MAP_KEY(AMPERSAND, 7)
    MAP_KEY(QUOTE, OEM_7)
    MAP_KEY(LEFTPAREN, 9)
    MAP_KEY(RIGHTPAREN, 0)
    MAP_KEY(ASTERISK, 8)
    MAP_KEY(PLUS, OEM_PLUS)
    MAP_KEY(COMMA, OEM_COMMA)
    MAP_KEY(MINUS, OEM_MINUS)
    MAP_KEY(PERIOD, OEM_PERIOD)
    MAP_KEY(SLASH, OEM_2)
    MAP_KEY(0, 0)
    MAP_KEY(1, 1)
    MAP_KEY(2, 2)
    MAP_KEY(3, 3)
    MAP_KEY(4, 4)
    MAP_KEY(5, 5)
    MAP_KEY(6, 6)
    MAP_KEY(7, 7)
    MAP_KEY(8, 8)
    MAP_KEY(9, 9)
    MAP_KEY(COLON, OEM_1)
    MAP_KEY(SEMICOLON, OEM_1)
    MAP_KEY(LESS, OEM_COMMA)
    MAP_KEY(EQUALS, OEM_PLUS)
    MAP_KEY(GREATER, OEM_PERIOD)
    MAP_KEY(QUESTION, OEM_2)
    MAP_KEY(AT, 2)
    MAP_KEY(LEFTBRACKET, OEM_4)
    MAP_KEY(BACKSLASH, OEM_5)
    MAP_KEY(RIGHTBRACKET, OEM_6)
    MAP_KEY(CARET, 6)
    MAP_KEY(UNDERSCORE, OEM_MINUS)
    MAP_KEY(BACKQUOTE, OEM_3)
    MAP_KEY(a, A)
    MAP_KEY(b, B)
    MAP_KEY(c, C)
    MAP_KEY(d, D)
    MAP_KEY(e, E)
    MAP_KEY(f, F)
    MAP_KEY(g, G)
    MAP_KEY(h, H)
    MAP_KEY(i, I)
    MAP_KEY(j, J)
    MAP_KEY(k, K)
    MAP_KEY(l, L)
    MAP_KEY(m, M)
    MAP_KEY(n, N)
    MAP_KEY(o, O)
    MAP_KEY(p, P)
    MAP_KEY(q, Q)
    MAP_KEY(r, R)
    MAP_KEY(s, S)
    MAP_KEY(t, T)
    MAP_KEY(u, U)
    MAP_KEY(v, V)
    MAP_KEY(w, W)
    MAP_KEY(x, X)
    MAP_KEY(y, Y)
    MAP_KEY(z, Z)
    MAP_KEY(DELETE, DELETE)
    MAP_KEY(KP0, NUMPAD0)
    MAP_KEY(KP1, NUMPAD1)
    MAP_KEY(KP2, NUMPAD2)
    MAP_KEY(KP3, NUMPAD3)
    MAP_KEY(KP4, NUMPAD4)
    MAP_KEY(KP5, NUMPAD5)
    MAP_KEY(KP6, NUMPAD6)
    MAP_KEY(KP7, NUMPAD7)
    MAP_KEY(KP8, NUMPAD8)
    MAP_KEY(KP9, NUMPAD9)
    MAP_KEY(KP_PERIOD, DECIMAL)
    MAP_KEY(KP_DIVIDE, DIVIDE)
    MAP_KEY(KP_MULTIPLY, MULTIPLY)
    MAP_KEY(KP_MINUS, SUBTRACT)
    MAP_KEY(KP_PLUS, ADD)
    MAP_KEY(KP_ENTER, SEPARATOR)
    MAP_KEY(KP_EQUALS, UNKNOWN)
    MAP_KEY(UP, UP)
    MAP_KEY(DOWN, DOWN)
    MAP_KEY(RIGHT, RIGHT)
    MAP_KEY(LEFT, LEFT)
    MAP_KEY(INSERT, INSERT)
    MAP_KEY(HOME, HOME)
    MAP_KEY(END, END)
    MAP_KEY(PAGEUP, PRIOR)
    MAP_KEY(PAGEDOWN, NEXT)
    MAP_KEY(F1, F1)
    MAP_KEY(F2, F2)
    MAP_KEY(F3, F3)
    MAP_KEY(F4, F4)
    MAP_KEY(F5, F5)
    MAP_KEY(F6, F6)
    MAP_KEY(F7, F7)
    MAP_KEY(F8, F8)
    MAP_KEY(F9, F9)
    MAP_KEY(F10, F10)
    MAP_KEY(F11, F11)
    MAP_KEY(F12, F12)
    MAP_KEY(F13, F13)
    MAP_KEY(F14, F14)
    MAP_KEY(F15, F15)
    MAP_KEY(NUMLOCK, NUMLOCK)
    MAP_KEY(CAPSLOCK, CAPITAL)
    MAP_KEY(SCROLLOCK, SCROLL)
    MAP_KEY(RSHIFT, RSHIFT)
    MAP_KEY(LSHIFT, LSHIFT)
    MAP_KEY(RCTRL, RCONTROL)
    MAP_KEY(LCTRL, LCONTROL)
    MAP_KEY(RALT, RMENU)
    MAP_KEY(LALT, LMENU)
    MAP_KEY(RMETA, LWIN)
    MAP_KEY(LMETA, RWIN)
    MAP_KEY(LSUPER, LWIN)
    MAP_KEY(RSUPER, RWIN)
    MAP_KEY(MODE, MODECHANGE)
    MAP_KEY(COMPOSE, ACCEPT)
    MAP_KEY(HELP, HELP)
    MAP_KEY(PRINT, SNAPSHOT)
    MAP_KEY(SYSREQ, EXECUTE)
    //default: return Awesomium::KeyCodes::AK_UNKNOWN;
    default: return AK_UNKNOWN;
    }
}

#undef MAP_KEY

static awe_mousebutton get_awe_mouse_button_from_SDL(Uint8 button)
{
    switch (button)
    {
        case SDL_BUTTON_LEFT:
            return AWE_MB_LEFT;

        case SDL_BUTTON_RIGHT:
            return AWE_MB_RIGHT;

        case SDL_BUTTON_WHEELUP:
        case SDL_BUTTON_WHEELDOWN:
        case SDL_BUTTON_MIDDLE:
            return AWE_MB_MIDDLE;

        default:
            GS_ASSERT(false);
            return AWE_MB_LEFT;
    }
    GS_ASSERT(false);
    return AWE_MB_LEFT;
}

void ChromeViewport::inject_SDL_key_event(const SDL_Event* event)
{
    if (event->type != SDL_KEYDOWN && event->type != SDL_KEYUP)
        return;

    awe_webkeyboardevent key_event;
    key_event.type = event->type == SDL_KEYDOWN ? AWE_WKT_KEYDOWN : AWE_WKT_KEYUP;

    key_event.virtual_key_code = get_web_key_from_SDL_key(event->key.keysym.sym);
    key_event.modifiers = 0;
    key_event.is_system_key = false;

    if (event->key.keysym.mod & KMOD_LALT || event->key.keysym.mod & KMOD_RALT)
        key_event.modifiers |= AWE_WKM_ALT_KEY;
    if (event->key.keysym.mod & KMOD_LCTRL || event->key.keysym.mod & KMOD_RCTRL)
        key_event.modifiers |= AWE_WKM_CONTROL_KEY;
    if (event->key.keysym.mod & KMOD_LMETA || event->key.keysym.mod & KMOD_RMETA)
        key_event.modifiers |= AWE_WKM_META_KEY;
    if (event->key.keysym.mod & KMOD_LSHIFT || event->key.keysym.mod & KMOD_RSHIFT)
        key_event.modifiers |= AWE_WKM_SHIFT_KEY;
    if (event->key.keysym.mod & KMOD_NUM)
        key_event.modifiers |= AWE_WKM_IS_KEYPAD;

    key_event.native_key_code = event->key.keysym.scancode;

    if (event->type == SDL_KEYUP)
        awe_webview_inject_keyboard_event(this->webView, key_event);
    else if (event->type == SDL_KEYDOWN)
    {
        unsigned int chr = event->key.keysym.unicode;
        if ((event->key.keysym.unicode & 0xFF80) == 0)
            chr = event->key.keysym.unicode & 0x7F;

        memset(key_event.text, '\0', sizeof(key_event.text));
        memset(key_event.unmodified_text, '\0', sizeof(key_event.unmodified_text));
        key_event.text[0] = chr;
        key_event.unmodified_text[0] = chr;

        awe_webview_inject_keyboard_event(this->webView, key_event);
        if (chr)
        {
            key_event.type = AWE_WKT_CHAR;
            key_event.virtual_key_code = chr;
            key_event.native_key_code = chr;
            awe_webview_inject_keyboard_event(this->webView, key_event);
        }
    }
}

void ChromeViewport::inject_SDL_mouse_event(const SDL_Event* event)
{
    if (event->button.button == SDL_BUTTON_WHEELDOWN || event->button.button == SDL_BUTTON_WHEELUP)
    {
        int horiz_scroll_amt = 0;
        int vert_scroll_amt = 28;
        if (event->button.button == SDL_BUTTON_WHEELDOWN)
            vert_scroll_amt *= -1;
        awe_webview_inject_mouse_wheel(this->webView, vert_scroll_amt, horiz_scroll_amt);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
        awe_webview_inject_mouse_down(this->webView, get_awe_mouse_button_from_SDL(event->button.button));
    else if (event->type == SDL_MOUSEBUTTONUP)
        awe_webview_inject_mouse_up(this->webView, get_awe_mouse_button_from_SDL(event->button.button));
    else if (event->type == SDL_MOUSEMOTION)
    {
        int x,y,sx,sy;
        SDL_GetMouseState(&x, &y);
        if (this->position_in_window(x, y, &sx, &sy))
            awe_webview_inject_mouse_move(this->webView, sx, sy);
    }
}

void ChromeViewport::process_key_event(const SDL_Event* event)
{
    IF_ASSERT(this->webView == NULL) return;

    if (!this->in_focus)
    {
        printf("Error? ChromeViewport::process_key_event, possible error. ChromeViewport received keyboard event but is not in focus\n");
        return;
    }

    this->inject_SDL_key_event(event);

    SDLKey key = event->key.keysym.sym;

    // Separate handling for history navigation -- awesomium does not do this by default
    if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.mod & (KMOD_LALT|KMOD_RALT))
        {
            if (key == SDLK_LEFT)
                awe_webview_go_to_history_offset(this->webView, -1);
            if (key == SDLK_RIGHT)
                awe_webview_go_to_history_offset(this->webView, 1);
        }

        //#if !PRODUCTION
        if (key == SDLK_MINUS)
        {
            char* token = get_auth_token();
            if (token == NULL)
                printf("No token found\n");
            else
                printf("Token: %s\n", token);
            free(token);
        }
        else
        if (key == SDLK_EQUALS)
        {
            open_token_page();
        }
        //#endif
    }
}


}   // Awesomium

#ifdef __clang__
# pragma clang diagnostic pop
#endif

#endif
