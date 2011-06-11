#include <SDL.h>
#include <SDL_opengl.h>

/*
SDL_Event Event;
Uint8 *keystate = SDL_GetKeyState(NULL);
SDL_PollEvent( &Event );
*/

SDL_Event Event;
Uint8 *keystate;

int _get_key() {

Uint8 *keystate = SDL_GetKeyState(NULL);
SDL_PollEvent( &Event );

if ( keystate[SDLK_UP] ) { printf( "Up Key" );}

return 0;
}
