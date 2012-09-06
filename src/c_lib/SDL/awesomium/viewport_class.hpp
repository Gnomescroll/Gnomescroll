#pragma once

#include <SDL/awesomium/_interface.hpp>
#include <auth/constants.hpp>
#include <auth/client.hpp>

namespace Awesomium
{

int getWebKeyFromSDLKey(SDLKey key);
void injectSDLKeyEvent(awe_webview* webView, const SDL_Event& event);

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

    check_for_token_cookie(_url);
}

void finish_loading_cb(awe_webview* webView)
{
    printf("Finish loading callback triggered\n");
    awe_string* _url = awe_webview_get_url(webView);
    char* url = get_str_from_awe(_url);
    printf("URL on webView is: %s\n", url);
    free(url);
    printf("\n");
}

awe_resource_response* resource_request_cb(awe_webview* webView, awe_resource_request* request)
{
    printf("Resource request callback triggered\n");
    awe_string* _url = awe_webview_get_url(webView);
    char* url = get_str_from_awe(_url);
    printf("URL on webView is: %s\n", url);
    free(url);
    printf("\n");
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

class ChromeViewport
{
    public: 
        int xoff;
        int yoff;
        int width;
        int height;

        bool inFocus;

        awe_webview* webView;
        unsigned int tex;

    ChromeViewport() :
    inFocus(false), tex(0)
    {
        this->xoff = (int) (_xresf * 0.125f);
        this->yoff = (int) (_yresf * 0.125f);    // from bottom
        this->width = (int) (_xresf * 0.75f);
        this->height = (int) (_yresf * 0.75f);

        this->init_webview();
        this->init_render_surface();

        this->set_callbacks();

        this->load_first_url();
    }

    void set_callbacks()
    {
        //awe_webview_set_callback_begin_navigation(this->webView, &begin_navigation_cb);
        awe_webview_set_callback_begin_loading(this->webView, &begin_loading_cb);
        //awe_webview_set_callback_finish_loading(this->webView, &finish_loading_cb);
        //awe_webview_set_callback_resource_response(this->webView, &resource_response_cb);
        //awe_webview_set_callback_resource_request(this->webView, &resource_request_cb);
    }

    void load_first_url()
    {
        this->load_url(GNOMESCROLL_URL);
    }

    void load_url(const char* url)
    {
        awe_string* a_url = get_awe_string(url);

        awe_webview_load_url(
            this->webView,
            a_url,  // url
            awe_string_empty(), // frame name
            awe_string_empty(), // username
            awe_string_empty()  // password
        );

        awe_string_destroy(a_url);
    }

    ~ChromeViewport()
    {
        if (this->webView != NULL) awe_webview_destroy(this->webView);
    }

    void init_webview()
    {
        this->webView = awe_webcore_create_webview(this->width, this->height, false);
        //default loading

        GS_ASSERT(this->webView != NULL);
        if (this->webView == NULL) return;

        awe_webview_set_transparent(this->webView, 1); ///preserve transpanency of window
        //Sets whether or not pages should be rendered with transparency preserved.
        //(ex, for pages with style="background-color:transparent")
    }

    void init_render_surface()
    {
        GS_ASSERT(this->tex == 0);
        const awe_renderbuffer* renderBuffer = awe_webview_render(webView);

        GS_ASSERT(renderBuffer != NULL);
        if (renderBuffer == NULL) return;
       
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &this->tex);

        glBindTexture(GL_TEXTURE_2D, this->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //awe_renderbuffer_get_buffer(renderBuffer),
        //awe_renderbuffer_get_width(renderBuffer),
        //awe_renderbuffer_get_height(renderBuffer),
        //awe_renderbuffer_get_rowspan(renderBuffer)

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*) awe_renderbuffer_get_buffer(renderBuffer) );

        glDisable(GL_TEXTURE_2D);

        CHECK_GL_ERROR();
    }

    void update_webview()
    {
        if(!awe_webview_is_dirty(this->webView) || awe_webview_is_loading_page(this->webView)) return;
        //awe_rect rect = awe_webview_get_dirty_bounds(webView);
        GS_ASSERT_LIMIT(this->tex != 0, 1);
        if (this->tex == 0) return;

        const awe_renderbuffer* renderBuffer = awe_webview_render(webView);

        GS_ASSERT(renderBuffer != NULL);
        if (renderBuffer == NULL) 
        {
            this->tex = 0;
            return;
        }

        glEnable(GL_TEXTURE_2D);
        glBindTexture( GL_TEXTURE_2D, tex );
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*) awe_renderbuffer_get_buffer(renderBuffer) );
        glDisable(GL_TEXTURE_2D);
    }

    void draw_webview()
    {
        if (this->tex == 0) return;

        GL_ASSERT(GL_DEPTH_TEST, false);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);

        static const float z = -0.5f;

        float x0 = xoff;
        float y0 = yoff;
        float x1 = xoff + width;
        float y1 = yoff + height;

        glBegin(GL_QUADS);

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(x0, y0, z);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(x1, y0, z);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(x1, y1, z);

        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x0, y1, z);

        glEnd();
        
        glDisable(GL_TEXTURE_2D);

        //draw point for debugging
        glColor4ub(255,0,0,255);
        glBegin(GL_POINTS);
        glVertex3f(xoff, yoff, -0.6f);
        glEnd();
        glColor4ub(255,255,255,255);
    }

    void focus()
    {
        awe_webview_focus(webView);
    }

    void unfocus()
    {
        awe_webview_unfocus(webView);
    }


    void set_html(char* html)
    {
        awe_string* html_str = awe_string_create_from_ascii(html, strlen(html));
        awe_webview_load_html(webView, html_str,awe_string_empty());
        awe_string_destroy(html_str);
    }

    void set_url(char* url) 
    {
        awe_string* url_str = awe_string_create_from_ascii(url, strlen(url));
        awe_webview_load_url(webView, url_str, awe_string_empty(), awe_string_empty(), awe_string_empty()); 
        awe_string_destroy(url_str);
    }


    void processKeyEvent(SDL_Event keyEvent) 
    {
        if(inFocus == false)
        {
            printf("Error? ChromeViewport::processKeyEvent, possible error. ChromeViewport received keyboard event but is not in focus\n");
            return;
        }
        injectSDLKeyEvent(webView, keyEvent);
    }
};

/*
_OSMExport bool awe_webview_is_dirty  (   awe_webview *   webview )
_OSMExport const awe_renderbuffer* awe_webview_render (   awe_webview *   webview )
_OSMExport awe_rect awe_webview_get_dirty_bounds  (   awe_webview *   webview )
Returns the bounds of the area that has changed since the last call to awe_webview_render.
*/

/*
_OSMExport int  awe_renderbuffer_get_width (const awe_renderbuffer *renderbuffer)
_OSMExport int  awe_renderbuffer_get_height (const awe_renderbuffer *renderbuffer)
_OSMExport int  awe_renderbuffer_get_rowspan (const awe_renderbuffer *renderbuffer)
_OSMExport const unsigned char *    awe_renderbuffer_get_buffer (const awe_renderbuffer *renderbuffer)
*/


/*
    // Destroy our WebView instance
    awe_webview_destroy(webView);

    // Destroy our WebCore instance
    awe_webcore_shutdown();
*/

/*        awe_string* filename_str = awe_string_create_from_ascii("./result.jpg",
                                                        strlen("./result.jpg"));

        // Save our RenderBuffer directly to a JPEG image
        awe_renderbuffer_save_to_jpeg(renderBuffer, filename_str, 90);

        // Destroy our filename string
        awe_string_destroy(filename_str);
*/


///input handling crap

// Forward declaration, defined below
//int getWebKeyFromSDLKey(SDLKey key);

/**
* This utility function generates a WebKeyboardEvent directly from an SDL
* Key Event and injects it into a certain WebView.
*
* @param    webView The WebView that will receive the event.
* @param    event   The SDL Event (of type SDL_KEYDOWN or SDL_KEYUP).
*/

//void injectSDLKeyEvent(Awesomium::WebView* webView, const SDL_Event& event)
//_awe_webkeyboardevent convertSDLKeyEvent(const SDL_Event& event)

//int getWebKeyFromSDLKey(SDLKey key);

awe_webkeyboardevent convert_key_event(Awesomium::WebKeyboardEvent keyEvent) 
{
    awe_webkeyboardevent ke;

/*
    awe_webkey_type     type
awe_webkey_modifiers    modifiers
int     virtual_key_code
int     native_key_code
wchar16     text [4]
wchar16     unmodified_text [4]
bool    is_system_keyURL
*/
    ke.type = (awe_webkey_type)keyEvent.type;
    ke.modifiers = (awe_webkey_modifiers)keyEvent.modifiers;
    ke.virtual_key_code = keyEvent.virtualKeyCode;
    ke.native_key_code = keyEvent.nativeKeyCode;
    int i;
    //for(i=0; i<=19;i++) {        ke.text[i] = keyEvent.keyIdentifier; }
    for(i=0; i<=3;i++) {
        ke.text[i] = keyEvent.text[i];
        ke.unmodified_text[i] = keyEvent.unmodifiedText[i];
    }
    ke.is_system_key = keyEvent.isSystemKey;
    return ke;
}


void injectSDLKeyEvent(awe_webview* webView, const SDL_Event* event)
{
    if(!(event->type == SDL_KEYDOWN || event->type == SDL_KEYUP))
        return;

    Awesomium::WebKeyboardEvent keyEvent;

    keyEvent.type = event->type == SDL_KEYDOWN?
        Awesomium::WebKeyboardEvent::TYPE_KEY_DOWN :
        Awesomium::WebKeyboardEvent::TYPE_KEY_UP;

    char* buf = new char[20];
    keyEvent.virtualKeyCode = getWebKeyFromSDLKey(event->key.keysym.sym);
    Awesomium::getKeyIdentifierFromVirtualKeyCode(keyEvent.virtualKeyCode,
                                                  &buf);
    strcpy(keyEvent.keyIdentifier, buf);
    delete[] buf;

    keyEvent.modifiers = 0;

    if(event->key.keysym.mod & KMOD_LALT || event->key.keysym.mod & KMOD_RALT)
        keyEvent.modifiers |= Awesomium::WebKeyboardEvent::MOD_ALT_KEY;
    if(event->key.keysym.mod & KMOD_LCTRL || event->key.keysym.mod & KMOD_RCTRL)
        keyEvent.modifiers |= Awesomium::WebKeyboardEvent::MOD_CONTROL_KEY;
    if(event->key.keysym.mod & KMOD_LMETA || event->key.keysym.mod & KMOD_RMETA)
        keyEvent.modifiers |= Awesomium::WebKeyboardEvent::MOD_META_KEY;
    if(event->key.keysym.mod & KMOD_LSHIFT || event->key.keysym.mod & KMOD_RSHIFT)
        keyEvent.modifiers |= Awesomium::WebKeyboardEvent::MOD_SHIFT_KEY;
    if(event->key.keysym.mod & KMOD_NUM)
        keyEvent.modifiers |= Awesomium::WebKeyboardEvent::MOD_IS_KEYPAD;

    keyEvent.nativeKeyCode = event->key.keysym.scancode;

    if(event->type == SDL_KEYUP)
    {
        ///webView->injectKeyboardEvent(keyEvent);
        awe_webview_inject_keyboard_event(webView, convert_key_event(keyEvent));
    }
    else
    {
        unsigned int chr;
        if((event->key.keysym.unicode & 0xFF80) == 0)
            chr = event->key.keysym.unicode & 0x7F;
        else
            chr = event->key.keysym.unicode;

        keyEvent.text[0] = chr;
        keyEvent.unmodifiedText[0] = chr;

        //webView->injectKeyboardEvent(keyEvent);
        awe_webview_inject_keyboard_event(webView, convert_key_event(keyEvent));
        if(chr)
        {
            keyEvent.type = Awesomium::WebKeyboardEvent::TYPE_CHAR;
            keyEvent.virtualKeyCode = chr;
            keyEvent.nativeKeyCode = chr;
            //webView->injectKeyboardEvent(keyEvent);
            awe_webview_inject_keyboard_event(webView, convert_key_event(keyEvent));
        }
    }
}

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

//_OSMExport void   awe_webview_inject_mouse_move (awe_webview *webview, int x, int y)
//_OSMExport void   awe_webview_inject_mouse_down (awe_webview *webview, awe_mousebutton button)
//_OSMExport void   awe_webview_inject_mouse_up (awe_webview *webview, awe_mousebutton button)
//_OSMExport void   awe_webview_inject_mouse_wheel (awe_webview *webview, int scroll_amount_vert, int scroll_amount_horz)
//_OSMExport void   awe_webview_inject_keyboard_event (awe_webview *webview, awe_webkeyboardevent key_event)

//enum      _awe_mousebutton { AWE_MB_LEFT, AWE_MB_MIDDLE, AWE_MB_RIGHT }

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
