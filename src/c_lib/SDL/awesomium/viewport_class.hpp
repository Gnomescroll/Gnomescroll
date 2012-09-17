#pragma once

#if GS_AWESOMIUM

#include <SDL/awesomium/_interface.hpp>
#include <auth/constants.hpp>
#include <auth/client.hpp>

namespace Awesomium
{

// Translates an SDLKey virtual key code to an Awesomium key code
int getWebKeyFromSDLKey(SDLKey key);
void injectSDLKeyEvent(awe_webview* webView, const SDL_Event* event);
bool get_webview_coordinates(class ChromeViewport* cv, int x, int y, int* sx, int* sy);
void injectSDLMouseEvent(awe_webview* webView, const SDL_Event* event);

const char JS_OBJ_NAME[] = "Gnomescroll";
const char JS_OBJ_CREATE_URL_NAME[] = "create_url";
const char JS_OBJ_LOGIN_URL_NAME[] = "login_url";
const char JS_OBJ_TOKEN_URL_NAME[] = "token_url";
const char JS_OBJ_LOGIN_ERROR_NAME[] = "login_error";
const char JS_OBJ_GAME_TOKEN_NAME[] = "gstoken";
const char JS_OBJ_DEBUG_NAME[] = "DEBUG";
const char JS_OBJ_TOKEN_NAME_NAME[] = "token_name";
const char JS_OBJ_USERNAME_NAME[] = "gs_username";
const char JS_OBJ_PASSWORD_NAME[] = "gs_pass";

// js -> C callbacks (registered on the Gnomescroll object)
const char JS_CB_SET_MESSAGE_NAME[] = "set_message";
const char JS_CB_UNSET_MESSAGE_NAME[] = "clear_message";
const char JS_CB_SET_TOKEN_NAME[] = "set_token";
const char JS_CB_TOKEN_FAILURE_NAME[] = "token_failure";
const char JS_CB_LOGIN_REQUIRED_NAME[] = "login_required";
const char JS_CB_SAVE_USERNAME_NAME[] = "save_username";
const char JS_CB_SAVE_PASSWORD_NAME[] = "save_password";

// C -> js callbacks (not registered, but defined in the js)
const char JS_CB_OPEN_TOKEN_PAGE_NAME[] = "gs_get_token";

void begin_navigation_cb(awe_webview* webView, const awe_string* _url, const awe_string* _frame_name);
void begin_loading_cb(awe_webview* webView, const awe_string* _url, const awe_string* _frame_name, int status_code, const awe_string* _mime_type);
void finish_loading_cb(awe_webview* webView);
awe_resource_response* resource_request_cb(awe_webview* webView, awe_resource_request* request);
void resource_response_cb(awe_webview* webView, const awe_string* _url, int status_code, bool was_cached, int64 request_time_ms, int64 response_time_ms, int64 expected_content_size, const awe_string *_mime_type);
void web_view_crashed_cb(awe_webview* webView);
void js_console_message_callback(awe_webview *webView, const awe_string *message, int line_number, const awe_string *source);

// main handler that routes to other callbacks
void js_callback_handler(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args);

// specific callbacks
void js_set_message_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args);
void js_unset_message_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args);
void js_set_token_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args);


class ChromeViewport
{
    public: 
        int xoff;
        int yoff;
        int width;
        int height;

        bool in_focus;

        awe_webview* webView;
        awe_string* js_obj_name;
        
        unsigned int tex;

        bool crashed;

    ChromeViewport() :
    in_focus(false), js_obj_name(NULL), tex(0), crashed(false)
    {
        this->xoff = (int) (_xresf * 0.125f);
        this->yoff = (int) (_yresf * 0.0625);    // from bottom
        this->width = (int) (_xresf * 0.75f);
        this->height = (int) (_yresf * 0.875f);

        this->init_webview();
        this->init_render_surface();

        //this->set_callbacks();
        //this->setup_whitelist();
        //this->setup_javascript();

        this->load_first_page();
    }

    ~ChromeViewport()
    {
        if (this->js_obj_name != NULL) awe_string_destroy(this->js_obj_name);
        if (this->webView != NULL) awe_webview_destroy(this->webView);
    }
    
    void set_callbacks();

    void add_site_to_whitelist(const char* _url)
    {
        GS_ASSERT(this->webView != NULL);
        if (this->webView == NULL) return;

        awe_string* url = get_awe_string(_url);
        awe_webview_add_url_filter(this->webView, url);
        awe_string_destroy(url);
    }

    void setup_whitelist()
    {
        GS_ASSERT(this->webView != NULL);
        if (this->webView == NULL) return;

        awe_webview_set_url_filtering_mode(this->webView, AWE_UFM_WHITELIST);
        this->add_site_to_whitelist("local://*");
        this->add_site_to_whitelist("file://*");
        this->add_site_to_whitelist ("http://gnomescroll.com/*");
        this->add_site_to_whitelist("https://gnomescroll.com/*");
        this->add_site_to_whitelist ("http://*.gnomescroll.com/*");
        this->add_site_to_whitelist("https://*.gnomescroll.com/*");
        //this->add_site_to_whitelist ("http://www.google-analytics.com/*");
        //this->add_site_to_whitelist("https://www.google-analytics.com/*");
        //this->add_site_to_whitelist ("http://ajax.googleapis.com/ajax/libs/jquery/*");
        //this->add_site_to_whitelist("https://ajax.googleapis.com/ajax/libs/jquery/*");
        #if !PRODUCTION
        this->add_site_to_whitelist ("http://127.0.0.1:5002/*");
        this->add_site_to_whitelist("https://127.0.0.1:5002/*");
        #endif
    }

    void setup_javascript()
    {
        GS_ASSERT(this->webView != NULL);
        if (this->webView == NULL) return;
        
        this->js_obj_name = get_awe_string(JS_OBJ_NAME);
        awe_webview_create_object(this->webView, this->js_obj_name);

        #if PRODUCTION
        this->set_js_value(JS_OBJ_DEBUG_NAME, true);
        #else
        this->set_js_value(JS_OBJ_DEBUG_NAME, false);
        #endif

        this->set_js_value(JS_OBJ_LOGIN_URL_NAME, GNOMESCROLL_LOGIN_URL);
        this->set_js_value(JS_OBJ_CREATE_URL_NAME, GNOMESCROLL_CREATE_URL);
        this->set_js_value(JS_OBJ_TOKEN_URL_NAME, GNOMESCROLL_TOKEN_URL);
        this->set_js_value(JS_OBJ_TOKEN_NAME_NAME, Auth::AUTH_TOKEN_COOKIE_NAME);

        // credentials
        char* username = NULL;
        char* password = NULL;
        get_credentials(&username, &password);
        if (username != NULL)
        {
            this->set_js_value(JS_OBJ_USERNAME_NAME, username);
            free(username);
            if (password != NULL)
            {
                this->set_js_value(JS_OBJ_PASSWORD_NAME, password);
                free(password);
            }
            else
                this->set_js_value(JS_OBJ_PASSWORD_NAME, "");
        }
        else
            this->set_js_value(JS_OBJ_USERNAME_NAME, "");
        
        // set some null values on the object
        this->set_js_value(JS_OBJ_LOGIN_ERROR_NAME);
        this->set_js_value(JS_OBJ_GAME_TOKEN_NAME);

        // callback for console.log
        awe_webview_set_callback_js_console_message(this->webView, &js_console_message_callback);

        this->register_js_callback(JS_CB_SET_MESSAGE_NAME);
        this->register_js_callback(JS_CB_UNSET_MESSAGE_NAME);
        this->register_js_callback(JS_CB_SET_TOKEN_NAME);
        this->register_js_callback(JS_CB_TOKEN_FAILURE_NAME);
        this->register_js_callback(JS_CB_LOGIN_REQUIRED_NAME);
        this->register_js_callback(JS_CB_SAVE_USERNAME_NAME);
        this->register_js_callback(JS_CB_SAVE_PASSWORD_NAME);
        
        // callbacks for error handling
        awe_webview_set_callback_js_callback(this->webView, &js_callback_handler);
    }

    void register_js_callback(const char* _name)
    {   // registers a javascript callback name on the global js obj
        GS_ASSERT(this->webView != NULL);
        GS_ASSERT(this->js_obj_name != NULL);
        if (this->webView == NULL || this->js_obj_name == NULL) return;
        
        awe_string* name = get_awe_string(_name);
        awe_webview_set_object_callback(this->webView, this->js_obj_name, name);
        awe_string_destroy(name);
    }

    void set_js_value(const char* _name, bool _value)
    {   // sets value with name to the global js obj
        GS_ASSERT(this->webView != NULL);
        GS_ASSERT(this->js_obj_name != NULL);
        if (this->webView == NULL || this->js_obj_name == NULL) return;

        awe_string* name = get_awe_string(_name);
        awe_jsvalue* value = awe_jsvalue_create_bool_value(_value);
        awe_webview_set_object_property(this->webView, this->js_obj_name, name, value);
        awe_string_destroy(name);
        awe_jsvalue_destroy(value);
    }
    
    void set_js_value(const char* _name, double _value)
    {   // sets value with name to the global js obj
        GS_ASSERT(this->webView != NULL);
        GS_ASSERT(this->js_obj_name != NULL);
        if (this->webView == NULL || this->js_obj_name == NULL) return;

        awe_string* name = get_awe_string(_name);
        awe_jsvalue* value = awe_jsvalue_create_double_value(_value);
        awe_webview_set_object_property(this->webView, this->js_obj_name, name, value);
        awe_string_destroy(name);
        awe_jsvalue_destroy(value);
    }
    
    void set_js_value(const char* _name, int _value)
    {   // sets value with name to the global js obj
        GS_ASSERT(this->webView != NULL);
        GS_ASSERT(this->js_obj_name != NULL);
        if (this->webView == NULL || this->js_obj_name == NULL) return;

        awe_string* name = get_awe_string(_name);
        awe_jsvalue* value = awe_jsvalue_create_integer_value(_value);
        awe_webview_set_object_property(this->webView, this->js_obj_name, name, value);
        awe_string_destroy(name);
        awe_jsvalue_destroy(value);
    }
    
    void set_js_value(const char* _name)
    {   // sets value with name to the global js obj
        GS_ASSERT(this->webView != NULL);
        GS_ASSERT(this->js_obj_name != NULL);
        if (this->webView == NULL || this->js_obj_name == NULL) return;

        awe_string* name = get_awe_string(_name);
        awe_jsvalue* value = awe_jsvalue_create_null_value();
        awe_webview_set_object_property(this->webView, this->js_obj_name, name, value);
        awe_string_destroy(name);
        awe_jsvalue_destroy(value);
    }
    
    void set_js_value(const char* _name, const char* _value)
    {   // sets value with name to the global js obj
        GS_ASSERT(this->webView != NULL);
        GS_ASSERT(this->js_obj_name != NULL);
        if (this->webView == NULL || this->js_obj_name == NULL) return;

        awe_string* name = get_awe_string(_name);
        awe_string* val = get_awe_string(_value);
        awe_jsvalue* value = awe_jsvalue_create_string_value(val);
        awe_webview_set_object_property(this->webView, this->js_obj_name, name, value);
        awe_string_destroy(name);
        awe_string_destroy(val);
        awe_jsvalue_destroy(value);
    }

    void load_first_page()
    {
        this->load_file(GNOMESCROLL_LOGIN_HTML);
    }

    void load_url(const char* _url)
    {
        GS_ASSERT(this->webView != NULL);
        if (this->webView == NULL) return;

        awe_string* url = get_awe_string(_url);
        awe_webview_load_url(
            this->webView,
            url,  // url
            awe_string_empty(), // frame name
            awe_string_empty(), // username
            awe_string_empty()  // password
        );
        awe_string_destroy(url);
    }

    void load_file(const char* _file)
    {
        GS_ASSERT(this->webView != NULL);
        if (this->webView == NULL) return;
        awe_string* file = get_awe_string(_file);
        awe_webview_load_file(
            this->webView,
            file,               // filename
            awe_string_empty()  // frame name
        );
        awe_string_destroy(file);
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*) awe_renderbuffer_get_buffer(renderBuffer));
        glDisable(GL_TEXTURE_2D);
    }

    void draw_webview()
    {
        if (this->tex == 0) return;

        glColor4ub(255,255,255,255);

        GL_ASSERT(GL_DEPTH_TEST, false);
        GL_ASSERT(GL_BLEND, true);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, this->tex);

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
    }

    void focus()
    {
        GS_ASSERT_LIMIT(this->webView != NULL, 1);
        if (this->webView == NULL) return;
        awe_webview_focus(this->webView);
        this->in_focus = true;
        printf("focus\n");
    }

    void unfocus()
    {
        GS_ASSERT_LIMIT(this->webView != NULL, 1);
        if (this->webView == NULL) return;
        awe_webview_unfocus(this->webView);
        this->in_focus = false;
        printf("unfocus\n");
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


    void processKeyEvent(const SDL_Event* event) 
    {
        GS_ASSERT_LIMIT(this->webView != NULL, 1);
        if (this->webView == NULL) return;
        
        if (!this->in_focus)
        {
            printf("Error? ChromeViewport::processKeyEvent, possible error. ChromeViewport received keyboard event but is not in focus\n");
            return;
        }
        
        injectSDLKeyEvent(this->webView, event);

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
    
            #if !PRODUCTION
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
            #endif
        }
    }
};

}   // Awesomium

#endif
