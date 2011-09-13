
#include <compat_gl.h>

int _xres = 800;
int _yres = 600;
int _fullscreen = 0;

SDL_Surface *pSDLSurface;
SDL_VideoInfo *pSDLVideoInfo;

void _del_video() {
    //printf("SDL_functions.c: _del_video, gracefull shutdown\n");
    SDL_Quit();
    //return 0;
}

int _set_resolution(int xres, int yres, int fullscreen) {
    _xres = xres;
    _yres = yres;
    _fullscreen = fullscreen;
    return 0;
}

int _multisampling = 0;

int _init_video() {
    int nFlags;
    int value;
    SDL_Init( SDL_INIT_VIDEO ); // Initialise the SDL Video bit

    SDL_WM_SetCaption( "SDL + OpenGL", NULL );
    ///const SDL_VideoInfo *pSDLVideoInfo = SDL_GetVideoInfo();
    pSDLVideoInfo = SDL_GetVideoInfo();

    if( !pSDLVideoInfo )
    {
        printf("SDL_GetVideoInfo() failed. SDL Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    nFlags = SDL_OPENGL; // | SDL_FULLSCREEN; //| SDL_GL_DOUBLEBUFFER; // | SDL_HWPALETTE;
    if(_fullscreen != 0) {nFlags |= SDL_FULLSCREEN; }

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

    if(0) //When the window is resized by the user a SDL_VIDEORESIZE event is generated and SDL_SetVideoMode can be called again with the new size.
        nFlags |= SDL_RESIZABLE;

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

    if( !pSDLSurface )
    {
        printf( "Call to SDL_SetVideoMode() failed! - SDL_Error: %s\n", SDL_GetError() );
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
    ///glEnable(GL_TEXTURE_2D); // ??? Needed?

    //whaa

    //glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    //glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
/*
    glEnable( GL_TEXTURE_2D );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
*/

    atexit(_del_video);

    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        printf("OpenGL 2.0 Supported \n");
    else {
        printf("OpenGL 2.0 not supported \n");
    }

    //multisample_test();
    //return 0;

    if(GLEW_ARB_multisample) {
        printf("ARB_MULTISAMPLE supported \n");
    } else {
        printf("ARB_MULTISAMPLE not supported \n");
    }
    return 0;
}


int _swap_buffers() {
    SDL_GL_SwapBuffers();
    //printf( "swap buffers\n");
    return 0;
}

int _get_ticks() {
return SDL_GetTicks();
}

