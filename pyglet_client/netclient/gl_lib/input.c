#include "input.h"

//event globals
SDL_Event Event;
Uint8 *keystate;

//input modes
int text_entry_mode;
//mouse globals
int mouse_x_last;
int mouse_y_last;

int _init_input() {
_set_text_entry_mode(1); ///change to 0 in production
mouse_x_last = 0;
mouse_y_last = 0;
}

int _set_text_entry_mode(int n) {
    if((n != 0) | (n != 1)) { text_entry_mode = n; } else { printf("input.c, _set_text_entry_mode error: mode invalid \n"); }
    return 0;
}

int _get_key_state() {
    keystate = SDL_GetKeyState(NULL);
    if ( keystate[SDLK_UP] ) { printf( "Up Key" );}
    return 0;
}

int _get_key_event() {
    SDL_EnableUNICODE( SDL_ENABLE );

    while(SDL_PollEvent( &Event )) { //returns 0 if no event
    //SDL_PumpEvents();

    switch( Event.type )
    {
        case SDL_QUIT:
            printf( "quit" );
            break;

        case SDL_KEYDOWN:
            if(text_entry_mode == 1) {
            printf("%c\n", getUnicodeValue(Event.key.keysym)); //This is for typing
            //printf("The %s key was pressed!\n", SDL_GetKeyName(Event.key.keysym.sym)); //This is for...
            } else {
            break;
            }

            break;

        case SDL_MOUSEBUTTONDOWN:
        {
            switch( Event.button.button )
            {
                case SDL_BUTTON_LEFT:
                    printf( "left mouse" );
                    break;
                    //g_nLastMousePositX = Event.motion.x;
                    //g_nLastMousePositY = Event.motion.y;
                    //g_bMousing = true;
            }
        }
        break;

        case SDL_MOUSEBUTTONUP:
        {
            switch( Event.button.button )
            {
              case SDL_BUTTON_LEFT:
                    //g_bMousing = false;
                    break;
            }
        }
        break;

        case SDL_MOUSEMOTION:
        {
            MouseState ms;
            ms.x = Event.motion.x;
            ms.y = Event.motion.y;
            ms.dx = Event.motion.xrel;
            ms.dy = Event.motion.yrel;
            //printf("Current mouse position is: (%d, %d)\n", Event.motion.x, Event.motion.y);
            break;
        }
        break;

        default: break;
    }
}

//SDL_EnableUNICODE( SDL_DISABLE );
}

int poll_events() {
    //mouse events
    //system events
    //keyboard events
    return 0;
}
/// Call Backs ///

int _key_state_callback(key_state_func user_func) {
    return 0;
}

int _key_event_callback(key_event_func user_func, char key) {
    user_func(key);
}

int _mouse_state_callback(mouse_state_func user_func, MouseState ms) {
    user_func(ms)
}

int _mouse_event_callback(mouse_event_func user_func, MouseEvent me) {
    user_func(me)
}

/// End Call Backs ///


char getUnicodeValue(SDL_keysym &keysym ) {
    // magic numbers courtesy of SDL docs :)
    const int INTERNATIONAL_MASK = 0xFF80, UNICODE_MASK = 0x7F;
    int uni = keysym.unicode;
            if( uni == 0 ) // not translatable key (like up or down arrows)
            {
                //SDL_GetKeyName( key );
                return 0;
            }
            if( ( uni & INTERNATIONAL_MASK ) == 0 )
            {
                if( SDL_GetModState() & KMOD_SHIFT )
                {
                    return (char)(toupper(uni & UNICODE_MASK));
                }
                else
                {
                    return (char)(uni & UNICODE_MASK);
                }
            }
            else // we have a funky international character. one we can't read :(
            {        // we could do nothing, or we can just show some sign of input, like so:
            return '?';
            }
}

