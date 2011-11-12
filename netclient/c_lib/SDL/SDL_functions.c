

#include <compat_gl.h>

#ifdef _WIN32
    #include "windows.h"
#endif

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

    printf("SDL: %s\n", SDL_GetError());

    SDL_WM_SetCaption( "Gnomescroll", NULL );
    const SDL_VideoInfo *pSDLVideoInfo = SDL_GetVideoInfo();
    //pSDLVideoInfo = SDL_GetVideoInfo();
    printf("SDL_GetVideoInfo: %s\n", SDL_GetError());

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
    printf("SDL_SetVideoMode: %s\n", SDL_GetError());

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

    atexit(_del_video);

    printf("SDL: %s\n", SDL_GetError());

    printf("glew init\n");
    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        printf("OpenGL 2.0 Supported \n");
    else {
        printf("OpenGL 2.0 not supported \n");
    }

    if(GLEW_ARB_multisample) {
        printf("ARB_MULTISAMPLE supported \n");
    } else {
        printf("ARB_MULTISAMPLE not supported \n");
    }


    if(GLEW_GL_EXT_timer_query) {
        printf("GL_EXT_timer_query supported\n");
    } else {
        printf("GL_EXT_timer_query not supported\n");
    }


    printf("SDL: %s\n", SDL_GetError());

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
    int _time2;

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

//  for(i=0; i < 16; i++) {
//      s_buffer[(16 + s_index - i) % 16)
//{
