#include once

#include <Awesomium/awesomium_capi.h>
#include <Awesomium/WebCore.h>

namespace Awesonium
{

void injectSDLKeyEvent(awe_webview* webView, const SDL_Event& event);
///linux only
//#include <unistd.h>

#define URL "http://www.google.com"


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



void libChromeShutdown() 
{
    //printf("libChrome shutdown\n");
    return;
}

extern "C" int _init2() {
    awe_webcore_initialize_default();
    atexit(libChromeShutdown);
    return 0;
}

extern "C" int _update() {
    awe_webcore_update(); //update web core; all windows
    return 0;
}


class chrome_viewport
{
	public:	
		int xoff;
		int yoff;
		int width;
		int height;

		bool inFocus;

		int texture_id;

		struct awe_webview* webView;
		unsigned int TEX0;

		chrome_viewport()
		{
			inFocus = false;

			xoff = 128;
			yoff = 128;
			width = 256;
			height = 256;

			init_webview();
		}

		~chrome_viewport()
		{
			awe_webview_destroy(webView);
		}

		void init_webview()
		{
			webView = awe_webcore_create_webview(width, height);
		    //default loading
		    awe_webview_set_transparent(webView, 1); ///preserve transpanency of window
		    //Sets whether or not pages should be rendered with transparency preserved.
		    //(ex, for pages with style="background-color:transparent")

		#if 1
		    awe_string* url_str = awe_string_create_from_ascii(URL, strlen(URL));
		    awe_webview_load_url(webView, url_str, awe_string_empty(),awe_string_empty(), awe_string_empty());
		    awe_string_destroy(url_str);
		#endif

		    struct awe_renderbuffer* renderBuffer = (awe_renderbuffer*) awe_webview_render(webView);

		    if(renderBuffer != NULL)
		    {
		    	printf("chrome_viewport: init_webview, error renderBuffer is null\n"); 
		    	return;
		    }
		   
		   	glEnable(GL_TEXTURE_2D);
		    glGenTextures(1, &TEX0);

			glBindTexture(GL_TEXTURE_2D, TEX0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		/*
	        display->surface = SDL_CreateRGBSurfaceFrom((void *) awe_renderbuffer_get_buffer(renderBuffer),
	                                awe_renderbuffer_get_width(renderBuffer),
	                                awe_renderbuffer_get_height(renderBuffer),
	                                32,
	                                awe_renderbuffer_get_rowspan(renderBuffer),
	                                0x0000FF00, 0x00FF0000,
	                                0xFF000000, 0x000000FF);
		*/

	        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, texture_format, GL_UNSIGNED_BYTE, (void*) awe_renderbuffer_get_buffer(renderBuffer) );

	        glDisable(GL_TEXTURE_2D);
		}

		void update_webview())
		{
		    if( !awe_webview_is_dirty(webView) ) return;

	        awe_rect rect = awe_webview_get_dirty_bounds(webView);

	        struct awe_renderbuffer* renderBuffer = (awe_renderbuffer*) awe_webview_render(webView);
	        if (renderBuffer == NULL) 
	        {
	        	printf("Chrome Crashed!\n"); return 0;}
	        glEnable(GL_TEXTURE_2D);
	        glBindTexture( GL_TEXTURE_2D, TEX0 );
	        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, display->surface->pixels );
	        glDisable(GL_TEXTURE_2D);
		}


		void draw_webview()
		{
		    glEnable(GL_TEXTURE_2D);
		    glBindTexture( GL_TEXTURE_2D, display->tex_id );

		    float z = -0.5;
		    float x0,y0,x1,y1;

		    x0 = xoff;
		    y0 = yoff;
		    x1 = xoff + width;
		    y1 = yoff + height;
		    //printf("(x0,y0,x1,t1= %f,%f,%f,%f \n", x0, y0, x1, y1);

		    glBegin( GL_QUADS );
		        glTexCoord2f( 0.0, 1.0);
		        glVertex3f( x0, y0, z );

		        glTexCoord2f( 1.0, 1.0 );
		        glVertex3f( x1, y0, z );

		        glVertex3f( x1, y1, z );

		        glTexCoord2f( 0.0, 0.0 );
		        glVertex3f( x0, y1, z );
		    glEnd();

		    glDisable(GL_TEXTURE_2D);
		}

		void focus()
		{
			awe_webview_focus(webView);
		}

		void unfocus()
		{
			awe_webview_unfocus(webView);
		}


		void set_html(char* html)() 
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
		    if(windowInFocus == false)  return;
		    injectSDLKeyEvent((awe_webview*)windowInFocus->webView, keyEvent);
		}


};

/*
struct chromeDisplay* windowInFocus;
struct chromeDisplay* windowInMouseFocus;


awe_webview* webView(chromeDisplay* display) 
{
    return (awe_webview*) display->webView;
}

int _set_window_focus(struct chromeDisplay* window) 
{
    windowInFocus = window;
    awe_webview_focus(webView(window));
    return 0;
}

int _defocus(void) {
    awe_webview_unfocus(webView(windowInFocus));
    windowInFocus = NULL;
    return 0;
}
*/

//_OSMExport void awe_webcore_set_base_directory    (   const awe_string *  base_dir_path   )
//for local assets

/*
int _setHTML(struct chromeDisplay* window, char* html) {
    awe_string* html_str = awe_string_create_from_ascii(html, strlen(html));
    awe_webview_load_html(webView(window), html_str,awe_string_empty());
    awe_string_destroy(html_str);
    return 0;
}

int _setURL(struct chromeDisplay* window, char* url) {
    awe_string* url_str = awe_string_create_from_ascii(url, strlen(url));

    awe_webview_load_url(webView(window), url_str, awe_string_empty(), awe_string_empty(), awe_string_empty()); 

    awe_string_destroy(url_str);
    return 0;
}
*/

/*
void injectSDLKeyEvent(awe_webview* webView, const SDL_Event& event); //forward declaration

void processKeyEvent(SDL_Event keyEvent) 
{
    if(windowInFocus == NULL) {
        //printf("No window in focus for keyboard event");
        return;
    }
    //printf("processKeyEvent: \n");
    injectSDLKeyEvent((awe_webview*)windowInFocus->webView, keyEvent);
}
*/

/*
struct chromeDisplay* _create_webview(int x,int y, int width, int height)
{
    chromeDisplay* display = (chromeDisplay*)malloc(sizeof(chromeDisplay));
    display->webView = (void*) awe_webcore_create_webview(width, height);
    display->tex_id = 0;
    display->x = x;
    display->y = y;
    display->width = width;
    display->height = height;
    //default loading
    awe_webview_set_transparent(webView(display), 1); ///preserve transpanency of window
    //Sets whether or not pages should be rendered with transparency preserved.
    //(ex, for pages with style="background-color:transparent")

    awe_string* url_str = awe_string_create_from_ascii(URL, strlen(URL));
    awe_webview_load_url(webView(display), url_str, awe_string_empty(),awe_string_empty(), awe_string_empty());
    awe_string_destroy(url_str);
    //end default loading

    awe_renderbuffer* renderBuffer = (awe_renderbuffer*) awe_webview_render(webView(display));

    if(renderBuffer != NULL)
    {
        printf("surface created\n");
        display->surface = SDL_CreateRGBSurfaceFrom((void *) awe_renderbuffer_get_buffer(renderBuffer),
                                    awe_renderbuffer_get_width(renderBuffer),
                                    awe_renderbuffer_get_height(renderBuffer),
                                    32,
                                    awe_renderbuffer_get_rowspan(renderBuffer),
                                    0x0000FF00, 0x00FF0000,
                                    0xFF000000, 0x000000FF);
    }
    else
    { printf("error creating Chrome surface\n"); }
    return display;
}
*/
//on_exit()) used by shared library to establish functions that are called when the shared library is unloaded.
//Functions registered using atexit() (and on_exit()) are not called if a process terminates abnormally because of the delivery of a signal

/*
void _delete_webview(struct chromeDisplay* display)
{
    awe_webview_destroy(webView(display));
    free(display);
}
*/
//Update an existing texture with glTexSubImage
//However, to resize a texture, you need to recreate it so glTexImage is your only option.

#if 0
void _update_webview(struct chromeDisplay* display)
{

	/*
		Create Texture
	*/
    if(display->tex_id == 0 )
    {
        awe_renderbuffer* renderBuffer = (awe_renderbuffer*) awe_webview_render(webView(display));
        if (renderBuffer == NULL) { printf("Chrome Crashed!\n"); return 0;}
        glEnable(GL_TEXTURE_2D);
        //if(display->tex_id != 0) { glDeleteTextures(1, &display->tex_id); }
        glGenTextures( 1, &display->tex_id );
        glBindTexture( GL_TEXTURE_2D, display->tex_id );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?
        //glTexImage2D(GL_TEXTURE_2D, 0, 4, display->surface->w, display->surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, display->surface->pixels );
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, display->surface->w, display->surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, display->surface->pixels );
        glDisable(GL_TEXTURE_2D);
    }

    /*
		Update Texture
    */
    if(awe_webview_is_dirty(webView(display)))
    {
        //printf("Is dirty\n");
        awe_rect rect = awe_webview_get_dirty_bounds(webView(display));
        awe_renderbuffer* renderBuffer = (awe_renderbuffer*) awe_webview_render(webView(display));
        if (renderBuffer == NULL) { printf("Chrome Crashed!\n"); return 0;}
        glEnable(GL_TEXTURE_2D);
        //if(display->tex_id != 0) { glDeleteTextures(1, &display->tex_id); }
        //glGenTextures( 1, &display->tex_id );
        glBindTexture( GL_TEXTURE_2D, display->tex_id );
        //glTexSubImage2D(GL_TEXTURE_2D, 0, rect.x, rect.y, rect.width, rect.height, GL_BGRA,GL_UNSIGNED_BYTE, display->surface->pixels);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, display->surface->w, display->surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, display->surface->pixels );
        glDisable(GL_TEXTURE_2D);
    } else {
       // printf("Is clean\n");
    }
    return 0;
}

int _draw_webview(struct chromeDisplay* display){
    if(display->tex_id == 0) { printf("tex_id == 0\n"); return 0;}
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, display->tex_id );

    float z = -0.5;
    float x0,y0,x1,y1;

    x0 = display->x;
    y0 = display->y;
    x1 = display->x + display->width;
    y1 = display->y + display->height;
    //printf("(x0,y0,x1,t1= %f,%f,%f,%f \n", x0, y0, x1, y1);

    glBegin( GL_QUADS );
        glTexCoord2f( 0.0, 1.0);
        glVertex3f( x0, y0, z );

        glTexCoord2f( 1.0, 1.0 );
        glVertex3f( x1, y0, z );

        glTexCoord2f( 1., 0. );
        glVertex3f( x1, y1, z );

        glTexCoord2f( 0., 0. );
        glVertex3f( x0, y1, z );
    glEnd();

    glDisable(GL_TEXTURE_2D);

return 0;
}
#endif

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
int getWebKeyFromSDLKey(SDLKey key);

inline awe_webkeyboardevent convert_key_event(Awesomium::WebKeyboardEvent keyEvent) 
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

void injectSDLKeyEvent(awe_webview* webView, const SDL_Event& event)
{
    if(!(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP))
        return;

    Awesomium::WebKeyboardEvent keyEvent;

    keyEvent.type = event.type == SDL_KEYDOWN?
        Awesomium::WebKeyboardEvent::TYPE_KEY_DOWN :
        Awesomium::WebKeyboardEvent::TYPE_KEY_UP;

    char* buf = new char[20];
    keyEvent.virtualKeyCode = getWebKeyFromSDLKey(event.key.keysym.sym);
    Awesomium::getKeyIdentifierFromVirtualKeyCode(keyEvent.virtualKeyCode,
                                                  &buf);
    strcpy(keyEvent.keyIdentifier, buf);
    delete[] buf;

    keyEvent.modifiers = 0;

    if(event.key.keysym.mod & KMOD_LALT || event.key.keysym.mod & KMOD_RALT)
        keyEvent.modifiers |= Awesomium::WebKeyboardEvent::MOD_ALT_KEY;
    if(event.key.keysym.mod & KMOD_LCTRL || event.key.keysym.mod & KMOD_RCTRL)
        keyEvent.modifiers |= Awesomium::WebKeyboardEvent::MOD_CONTROL_KEY;
    if(event.key.keysym.mod & KMOD_LMETA || event.key.keysym.mod & KMOD_RMETA)
        keyEvent.modifiers |= Awesomium::WebKeyboardEvent::MOD_META_KEY;
    if(event.key.keysym.mod & KMOD_LSHIFT || event.key.keysym.mod & KMOD_RSHIFT)
        keyEvent.modifiers |= Awesomium::WebKeyboardEvent::MOD_SHIFT_KEY;
    if(event.key.keysym.mod & KMOD_NUM)
        keyEvent.modifiers |= Awesomium::WebKeyboardEvent::MOD_IS_KEYPAD;

    keyEvent.nativeKeyCode = event.key.keysym.scancode;

    if(event.type == SDL_KEYUP)
    {
        ///webView->injectKeyboardEvent(keyEvent);
        awe_webview_inject_keyboard_event(webView, convert_key_event(keyEvent));
    }
    else
    {
        unsigned int chr;
        if((event.key.keysym.unicode & 0xFF80) == 0)
            chr = event.key.keysym.unicode & 0x7F;
        else
            chr = event.key.keysym.unicode;

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




}