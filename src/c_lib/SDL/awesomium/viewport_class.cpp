#include "viewport_class.hpp"

#if GS_AWESOMIUM

#include <hud/hud.hpp>
#include <chat/client.hpp>

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
    if (cv != NULL)
    {
        cv->crashed = true;
        cv->webView = NULL;
    }
    printf("Webview crashed\n");
}

void js_set_message_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    const awe_jsvalue* vmsg = awe_jsarray_get_element(_args, 0);
    GS_ASSERT(vmsg != NULL);
    if (vmsg == NULL) return;
    
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
    GS_ASSERT(vtoken != NULL);
    if (vtoken == NULL) return;

    awe_string* _token = awe_jsvalue_to_string(vtoken);
    char* token = get_str_from_awe(_token);
    Auth::AuthError error = Auth::update_token(token);
    GS_ASSERT(error == Auth::AUTH_ERROR_NONE);
    if (error != Auth::AUTH_ERROR_NONE)
        Auth::token_failure = false;
    free(token);
    awe_string_destroy(_token);
}

void js_token_failure_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    Auth::token_failure = true;
}

void js_login_required_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args)
{
    Auth::needs_login = true;
    if (chat_client != NULL)
    {
        chat_client->send_system_message("There was a server reset. You will need to again soon to continue playing.");
        chat_client->send_system_message(Hud::open_login_text);
    }
    Hud::set_prompt(Hud::open_login_text);
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
        printf("Unhandled javascript callback triggered: %s\n", cb);
    free(cb);
}

void ChromeViewport::set_callbacks()
{
    //awe_webview_set_callback_begin_navigation(this->webView, &begin_navigation_cb);
    //awe_webview_set_callback_begin_loading(this->webView, &begin_loading_cb);
    //awe_webview_set_callback_finish_loading(this->webView, &finish_loading_cb);
    //awe_webview_set_callback_resource_response(this->webView, &resource_response_cb);
    //awe_webview_set_callback_resource_request(this->webView, &resource_request_cb);
    awe_webview_set_callback_web_view_crashed(this->webView, &web_view_crashed_cb);
}

void injectSDLKeyEvent(awe_webview* webView, const SDL_Event* event)
{
    if (event->type != SDL_KEYDOWN && event->type != SDL_KEYUP)
        return;
        
    awe_webkeyboardevent key_event;
    key_event.type = event->type == SDL_KEYDOWN ? AWE_WKT_KEYDOWN : AWE_WKT_KEYUP;

    key_event.virtual_key_code = getWebKeyFromSDLKey(event->key.keysym.sym);
    key_event.modifiers = 0;

    if(event->key.keysym.mod & KMOD_LALT || event->key.keysym.mod & KMOD_RALT)
        key_event.modifiers |= AWE_WKM_ALT_KEY;
    if(event->key.keysym.mod & KMOD_LCTRL || event->key.keysym.mod & KMOD_RCTRL)
        key_event.modifiers |= AWE_WKM_CONTROL_KEY;
    if(event->key.keysym.mod & KMOD_LMETA || event->key.keysym.mod & KMOD_RMETA)
        key_event.modifiers |= AWE_WKM_META_KEY;
    if(event->key.keysym.mod & KMOD_LSHIFT || event->key.keysym.mod & KMOD_RSHIFT)
        key_event.modifiers |= AWE_WKM_SHIFT_KEY;
    if(event->key.keysym.mod & KMOD_NUM)
        key_event.modifiers |= AWE_WKM_IS_KEYPAD;

    key_event.native_key_code = event->key.keysym.scancode;

    if (event->type == SDL_KEYUP)
        awe_webview_inject_keyboard_event(webView, key_event);
    else if (event->type == SDL_KEYDOWN)
    {
        unsigned int chr = event->key.keysym.unicode;
        if ((event->key.keysym.unicode & 0xFF80) == 0)
            chr = event->key.keysym.unicode & 0x7F;

        key_event.text[0] = chr;
        key_event.text[1] = '\0';
        key_event.unmodified_text[0] = chr;
        key_event.unmodified_text[1] = '\0';

        awe_webview_inject_keyboard_event(webView, key_event);
        if (chr)
        {
            key_event.type = AWE_WKT_CHAR;
            key_event.virtual_key_code = chr;
            key_event.native_key_code = chr;
            awe_webview_inject_keyboard_event(webView, key_event);
        }
    }
}

// A helper macro, used in 'getWebKeyFromSDLKey'
//#define MAP_KEY(a, b) case SDLK_##a: return Awesomium::KeyCodes::AK_##b;
#define MAP_KEY(a, b) case SDLK_##a: return AK_##b;

// Translates an SDLKey virtual key code to an Awesomium key code
int getWebKeyFromSDLKey(SDLKey key)
{
    switch(key)
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

bool get_webview_coordinates(class ChromeViewport* cv, int x, int y, int* sx, int* sy)
{
    GS_ASSERT_LIMIT(cv != NULL, 1);
    if (cv == NULL) return false;

    // subtract top left of webview window
    int left = cv->xoff;
    int top = _yres - (cv->yoff + cv->height);

    *sx = x - left;
    *sy = y - top;

    return ((*sx) >= 0 && (*sx) < cv->width && (*sy) >= 0 && (*sy) < cv->height);
}

void injectSDLMouseEvent(awe_webview* webView, const SDL_Event* event)
{    
    if (event->button.button == SDL_BUTTON_WHEELDOWN || event->button.button == SDL_BUTTON_WHEELUP)
    {
        int horiz_scroll_amt = 0;
        int vert_scroll_amt = 28;
        if (event->button.button == SDL_BUTTON_WHEELDOWN)
            vert_scroll_amt *= -1;
        awe_webview_inject_mouse_wheel(webView, vert_scroll_amt, horiz_scroll_amt);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
        awe_webview_inject_mouse_down(webView, get_awe_mouse_button_from_SDL(event->button.button));
    else if (event->type == SDL_MOUSEBUTTONUP)
        awe_webview_inject_mouse_up(webView, get_awe_mouse_button_from_SDL(event->button.button));
    else if (event->type == SDL_MOUSEMOTION)
    {
        int x,y,sx,sy;
        SDL_GetMouseState(&x, &y);
        if (get_webview_coordinates(cv, x,y, &sx, &sy))
            awe_webview_inject_mouse_move(webView, sx,sy);
    }
}

}   // Awesomium

#endif
