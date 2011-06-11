#include <SDL.h>
#include <SDL_opengl.h>

#include <SDL_keyboard.h>

//prototypes
int _init_input();
int _set_text_enty_mode(int n);
char getUnicodeValue(SDL_keysym keysym );
int _get_key_state();
int _get_key_event();

//event globals
SDL_Event Event;
Uint8 *keystate;

//input modes
int text_entry_mode;
//mouse globals
int mouse_x_last;
int mouse_y_last;

typedef struct {
  int x;
  int y;
  int dx;
  int dy;
} MouseState;

int _init_input() {
_set_text_entry_mode(0);
mouse_x_last = 0;
mouse_y_last = 0
}

int _set_text_entry_mode(int n) {
    if(n != 0 | n != 1) { text_entry_mode = n; } else { printf("input.c, _set_text_entry_mode error: mode invalid \n"); }
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
            printf("The %c \n", getUnicodeValue(Event.key.keysym)); //This is for typing
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
            m_uiMouseDX = event.motion.xrel;
            m_uiMouseDY = event.motion.yrel;
            break;
            //printf("Current mouse position is: (%d, %d)\n", Event.motion.x, Event.motion.y);
            break;
        }
        break;

        default: break;
    }
}

SDL_EnableUNICODE( SDL_DISABLE );
}

int poll_events() {
    //mouse events
    //system events
    //keyboard events
    return 0;
}

char getUnicodeValue(SDL_keysym keysym ) {
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

