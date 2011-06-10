#include <SDL.h>
#include <SDL_opengl.h>

/*
void opengl_test2(void) {

SDL_Init( SDL_INIT_VIDEO );

float XSize = 640, YSize = 480;
glMatrixMode (GL_PROJECTION);
glLoadIdentity ();
glOrtho (0, XSize, YSize, 0, 0, 1);
glMatrixMode (GL_MODELVIEW);

float z = -0.5;
float x = 50;
float y = 50;

glColor3f(0,0.5,1);

glBegin(GL_POLYGON);
glVertex3f(1+x,1+y,0+z);
glVertex3f(1+x,0+y,0+z);
glVertex3f(0+x,0+y,0+z);
glVertex3f(0+x,1+y,0+z);
glEnd();

glBegin(GL_TRIANGLES);

glVertex3f(-1.0f, -0.5f, -4.0f);    // lower left vertex
glVertex3f( 1.0f, -0.5f, -4.0f);    // lower right vertex
glVertex3f( 0.0f,  0.5f, -4.0f);    // upper vertex

glEnd();

SDL_GL_SwapBuffers();
//SwapBuffers();
return 3;
}
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
    return 0;
}

int _del_video() {
    SDL_Quit();
    return 0;
}

