#include <SDL.h>
#include <SDL_opengl.h>

// #include <SDL_ttf.h>

/*

SDL_GL_SwapBuffers();
//SwapBuffers();

*/

int _init_video() {

    SDL_Init( SDL_INIT_VIDEO ); // Initialise the SDL Video bit

    SDL_WM_SetCaption( "SDL + OpenGL", NULL );
    const SDL_VideoInfo *pSDLVideoInfo = SDL_GetVideoInfo();
    if( !pSDLVideoInfo )
    {
        printf("SDL_GetVideoInfo() failed. SDL Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    int nFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE;
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

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ); // Enable OpenGL Doublebuffering

    // Create our rendering surface
    SDL_Surface *pSDLSurface = SDL_SetVideoMode( 800, 600, 32, nFlags );

    if( !pSDLSurface )
    {
        printf( "Call to SDL_SetVideoMode() failed! - SDL_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        return 1;
    }
    //glEnable(GL_TEXTURE_2D);
    return 0;

}

int _del_video() {
    SDL_Quit();
    return 0;
}

int _swap_buffers() {
    SDL_GL_SwapBuffers();
}
