#include "SDL_functions.h"

#include <compat_gl.h>

#ifdef _WIN32
    #include "windows.h"
#endif

#ifndef __APPLE__
    #include <c_lib/SDL/IMG_savepng.h>
#endif
/*
    Example code from
    http://www.opengl.org/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)
*/

/*
void setupwindow(SDL_WindowID *window, SDL_GLContext *context)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize SDL's Video subsystem 
        sdldie("Unable to initialize SDL"); // Or die on error
 
    // Request an opengl 3.2 context.
   //SDL doesn't have the ability to choose which profile at this time of writing,
    //but it should default to the core profile 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
 
    // Turn on double buffering with a 24bit Z buffer.
    // You may need to change this to 16 or 32 for your system
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
 
    //Create our window centered at 512x512 resolution 
    *window = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!*window) // Die if creation failed 
        sdldie("Unable to create window");
 
    //Create our opengl context and attach it to our window 
    *context = SDL_GL_CreateContext(*window);
 
    // This makes our buffer swap syncronized with the monitor's vertical refresh 
    SDL_GL_SetSwapInterval(1);
}
*/


int DisplayBox()
{
#ifdef _WIN32
    int msgboxID = MessageBox(
        NULL,
        (LPCSTR)L"Error: check console log",
        (LPCSTR)L"Possible Error",
        MB_ICONWARNING | MB_DEFBUTTON2
    );

    switch (msgboxID)
    {
    case IDCANCEL:
        // TODO: add code
        break;
    }

    return msgboxID;
#endif
    return 0;
}

int _xres = 800;
int _yres = 600;
float _xresf = (float) _xres;
float _yresf = (float) _yres;
int _fullscreen = 0;

SDL_Surface *pSDLSurface;
SDL_VideoInfo *pSDLVideoInfo;

void _del_video() {

    static int SDL_CLOSED = 0;
    if(SDL_CLOSED == 1)
    {
        printf("Warning! SDL un-init function called twiced! \n");
        return;
    }
    SDL_CLOSED = 1;
        
    printf("Deconstructing SDL OpenGL Window\n");
    _del_video();
    
    //printf("SDL_functions.c: _del_video, gracefull shutdown\n");
    SDL_Quit();
}

int _set_resolution(int xres, int yres, int fullscreen) {
    _xres = xres;
    _yres = yres;
    _xresf = (float)_xres;
    _yresf = (float)_yres;
    _fullscreen = fullscreen;
    return 0;
}

int _multisampling = 0;

void ati_meminfo(void)
{

    //printf("OpenGL: %s \n", (char*)glGetString(GL_VERSION));
    //printf("GPU: \n", (char*)glGetString(GL_RENDERER));
    //printf("Extentions= %s \n", (char*)glGetString(GL_EXTENSIONS));

    printf("\n");

    GLint param[4];

    glGetIntegerv (GL_VBO_FREE_MEMORY_ATI, param);
    printf ("VBO_FREE_MEMORY_ATI\n");
    printf ("  Total memory free in the pool: %i Kbyte\n", param[0]);
    printf ("  Largest available free block in the pool: %i Kbyte\n", param[1]);
    printf ("  Total auxiliary memory free: %i Kbyte\n", param[2]);
    printf ("  Largest auxiliary free block: %i Kbyte\n", param[3]);
    printf ("\n");

    glGetIntegerv (GL_TEXTURE_FREE_MEMORY_ATI, param);
    printf ("TEXTURE_FREE_MEMORY_ATI\n");
    printf ("  Total memory free in the pool: %i Kbyte\n", param[0]);
    printf ("  Largest available free block in the pool: %i Kbyte\n", param[1]);
    printf ("  Total auxiliary memory free: %i Kbyte\n", param[2]);
    printf ("  Largest auxiliary free block: %i Kbyte\n", param[3]);
    printf ("\n");

    glGetIntegerv (GL_RENDERBUFFER_FREE_MEMORY_ATI, param);
    printf ("RENDERBUFFER_FREE_MEMORY_ATI\n");
    printf ("  Total memory free in the pool: %i Kbyte\n", param[0]);
    printf ("  Largest available free block in the pool: %i Kbyte\n", param[1]);
    printf ("  Total auxiliary memory free: %i Kbyte\n", param[2]);
    printf ("  Largest auxiliary free block: %i Kbyte\n", param[3]);
    printf ("\n");

    return;
}


int init_video() {

    //DisplayBox();

    int nFlags;
    int value;

    printf("Creating SDL window\n");
    //SDL_Init( SDL_INIT_VIDEO ); // Initialise the SDL Video bit

    //-1 on error, 0 on success
    if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) == -1) {
       // Unrecoverable error, exit here.
       printf("SDL_Init failed: %s\n", SDL_GetError());
       DisplayBox();
       return 1;
    }

    //printf("SDL: %s\n", SDL_GetError());

    SDL_WM_SetCaption( "Gnomescroll", NULL );
    const SDL_VideoInfo *pSDLVideoInfo = SDL_GetVideoInfo();
    //pSDLVideoInfo = SDL_GetVideoInfo();
    //printf("SDL_GetVideoInfo: %s\n", SDL_GetError());

    if( !pSDLVideoInfo )
    {
        printf("SDL_GetVideoInfo() failed. SDL Error: %s\n", SDL_GetError());
        DisplayBox();
        SDL_Quit();
        return 1;
    }
    nFlags = SDL_OPENGL; // | SDL_FULLSCREEN; //| SDL_GL_DOUBLEBUFFER; // | SDL_HWPALETTE;
    if(_fullscreen != 0) {nFlags |= SDL_FULLSCREEN; }
/*
    if( pSDLVideoInfo->hw_available ) // Hardware surfaces enabled?
        nFlags |= SDL_HWSURFACE;
    else {
        nFlags |= SDL_SWSURFACE;
        printf( "SDL_HWSURFACE Error: Hardware surfaces not enabled!\n");
    }
    if( pSDLVideoInfo->blit_hw ) // Hardware supported blitting?
        nFlags |= SDL_HWACCEL;
    else
        printf( "SDL_HWACCEL Error: Hardware blitting not enabled!\n");
*/

    if(0) {//When the window is resized by the user a SDL_VIDEORESIZE event is generated and SDL_SetVideoMode can be called again with the new size.
        nFlags |= SDL_RESIZABLE;
    }

    nFlags |= SDL_DOUBLEBUF; // http://sdl.beuc.net/sdl.wiki/SDL_Flip

    if(_multisampling) {
        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ); // Enable OpenGL Doublebuffering
    }
    ///multisampling: no effect
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); //multisampling?
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    /// vsync
    //SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
    //may only work in full screen mode
    //deprecated

    /// anti-aliasing
    //SDL_GL_MULTISAMPLEBUFFERS
    //SDL_GL_MULTISAMPLESAMPLES

    // Create our rendering surface
    ///SDL_Surface *pSDLSurface = SDL_SetVideoMode( 800, 600, 32, nFlags );
    //pSDLSurface = SDL_SetVideoMode( 800, 600, 32, nFlags );
    pSDLSurface = SDL_SetVideoMode( _xres, _yres, 32, nFlags );
    //printf("SDL_SetVideoMode: %s\n", SDL_GetError());

    if( !pSDLSurface )
    {
        printf( "Call to SDL_SetVideoMode() failed! - SDL_Error: %s\n", SDL_GetError() );
        DisplayBox();
        SDL_Quit();
        return 1;
    }
    SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &value);
    if(value) {
        printf("Harware Acceleration Enabled \n");
    } else {
        printf("Warning: Hardware Acceleration Not Enabled!\n");
    }

    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &value);
    if(value) {
        printf("Double Buffering Enabled \n");
    } else {
        printf("Warning: Double Buffering Not Enabled!\n");
    }

    if(_multisampling) {
        SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &value);
        printf("Multisampling: number of samples per pixel %i \n", value);
    } else {
        glDisable(GL_MULTISAMPLE);
    }
    SDL_ShowCursor(SDL_DISABLE);

    //atexit(_del_video);

    //printf("SDL: %s\n", SDL_GetError());

    printf("glew init\n");
    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        printf("OpenGL 2.0 Supported \n");
    else {
        printf("OpenGL 2.0 not supported \n");
    }


    //const char* GLVersionString = (char *) glGetString(GL_VERSION);
    //printf("OpenGL version: %s \n", GLVersionString);

    printf("OpenGL: %s \n", (char*) glGetString(GL_VERSION));
    printf("GPU: %s \n", (char*) glGetString(GL_RENDERER));
    printf("Driver: %s \n", (char*) glGetString(GL_VENDOR));

    //printf("Extentions= %s \n", (char*)glGetString(GL_EXTENSIONS));

    //Or better yet, use the GL3 way to get the version number
    //int OpenGLVersion[2];
    //glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
    //glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);


    if(GLEW_ARB_multisample) {
        printf("ARB_MULTISAMPLE supported \n");
    } else {
        printf("ARB_MULTISAMPLE not supported \n");
    }

    if(GLEW_EXT_provoking_vertex) 
    {
        //glProvokingVertexEXT(GL_FIRST_VERTEX_CONVENTION_EXT);
    } else {
        printf("warning: ProvokingVertex extention not supported \n");

    }
    if(GLEW_EXT_timer_query) {
        printf("GL_EXT_timer_query supported\n");
    } else {
        printf("GL_EXT_timer_query not supported\n");
    }

    //printf("SDL: %s\n", SDL_GetError());

    //if(GLEW_ATI_meminfo) { ati_meminfo(); } 

    printf("Finished OpenGL init\n");

    glClearColor(0.0,0.0,0.0,0.0);

    return 0;
}

const int _L = 64;

static int _s_buffer[_L];
static int _s2_buffer[_L];
static int _s_index = 0;

static int _last_frame_t;

const int _SDL_SWAP_DEBUG = 0;

/*
    Measure the time since last frame and if there is extra time before next flip, do map processing

*/
int _swap_buffers() {
    int _time1;

    if(_SDL_SWAP_DEBUG)
    {
       _time1 = SDL_GetTicks();
    }
    
    // Do not call glFlush() before SDL_GL_SwapBuffers since the latter implies the former.

    //SDL_GL_SwapBuffers();
    //glFlush();
    //glFinish();
    SDL_GL_SwapBuffers();
    
    if(_SDL_SWAP_DEBUG)
    {
        int _time2 = SDL_GetTicks();

        _s_index = (_s_index + 1 )%_L;

        _s2_buffer[_s_index] = _time2 - _last_frame_t;
        _s_buffer[_s_index] = _time2 - _time1;

        _last_frame_t = _time2;
        int _flip_time = _time2 - _time1;


        if(_flip_time > 2) {
            printf("Warning: SDL buffer swap took %i ms\n", _flip_time);
            //can do stuff here!!!
            int i;
            for(i=0; i < 8; i++) {
                printf("frame %i: %i ms, %i ms\n", i, _s_buffer[(_L+_s_index-i)%_L], _s2_buffer[(_L+_s_index-i)%_L] );
                //printf("frame %i: %i ms, %i ms\n", i, _s_buffer[(_s_index+i)%_L], _s2_buffer[(_s_index+i)%_L] );
            }
        }
    }
    return 0;
}

int _get_ticks() {
    return SDL_GetTicks();
}

void close_SDL() 
{
    printf("close_SDL: Deconstructing SDL OpenGL Window\n");
    _del_video();
    
}

void save_screenshot()
{
#ifndef __APPLE__
    //int window_width = _xres;
    //int window_height = _yres;

    const char *FileName = "screenshot.png";

    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, _xres, _yres,
                                                   32, 0x0000ff, 0x00ff00, 0xff0000, 0x000000);


    SDL_LockSurface(surface);

    glReadPixels(0, 0, _xres, _yres, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    {
        int index;
        void* temp_row;
        int height_div_2;

        temp_row = (void *)malloc(surface->pitch);
        if(NULL == temp_row)
        {
            SDL_SetError("save_screenshot: not enough memory for surface inversion");
        }
        height_div_2 = (int) (surface->h * .5);
        for(index = 0; index < height_div_2; index++)    
        {
            memcpy((Uint8 *)temp_row,(Uint8 *)(surface->pixels) + surface->pitch * index, surface->pitch);
            memcpy((Uint8 *)(surface->pixels) + surface->pitch * index, (Uint8 *)(surface->pixels) + surface->pitch * (surface->h - index-1), surface->pitch);
            memcpy((Uint8 *)(surface->pixels) + surface->pitch * (surface->h - index-1), temp_row, surface->pitch);
        }
        free(temp_row); 
    }
    SDL_UnlockSurface(surface);
    //unsigned int *pixels = new unsigned int[window.width * window.height];
    //unsigned int *pixelsbuf = new unsigned int[window.width * window.height];
    
    IMG_SavePNG(FileName, surface, -1);

    //SDL_SaveBMP(surface, FileName);
    SDL_FreeSurface(surface);
#endif
}
