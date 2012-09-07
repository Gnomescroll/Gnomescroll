#pragma once

#include <SDL/awesomium/_interface.hpp>
#include <auth/constants.hpp>
#include <auth/client.hpp>

namespace Awesomium
{

int getWebKeyFromSDLKey(SDLKey key);
void injectSDLKeyEvent(awe_webview* webView, const SDL_Event& event);

void begin_navigation_cb(awe_webview* webView, const awe_string* _url, const awe_string* _frame_name);
void begin_loading_cb(awe_webview* webView, const awe_string* _url, const awe_string* _frame_name, int status_code, const awe_string* _mime_type);
void finish_loading_cb(awe_webview* webView);
awe_resource_response* resource_request_cb(awe_webview* webView, awe_resource_request* request);
void resource_response_cb(awe_webview* webView, const awe_string* _url, int status_code, bool was_cached, int64 request_time_ms, int64 response_time_ms, int64 expected_content_size, const awe_string *_mime_type);
void web_view_crashed_cb(awe_webview* webView);

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

        bool crashed;

    ChromeViewport() :
    inFocus(false), tex(0), crashed(false)
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

    void set_callbacks();

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
        GL_ASSERT(GL_BLEND, true);

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

// Translates an SDLKey virtual key code to an Awesomium key code
int getWebKeyFromSDLKey(SDLKey key);
awe_webkeyboardevent convert_key_event(Awesomium::WebKeyboardEvent keyEvent); 
void injectSDLKeyEvent(awe_webview* webView, const SDL_Event* event);
bool get_webview_coordinates(class ChromeViewport* cv, int x, int y, int* sx, int* sy);
void injectSDLMouseEvent(awe_webview* webView, const SDL_Event* event);

}   // Awesomium
