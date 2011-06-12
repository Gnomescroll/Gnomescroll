#include "./input.h"

//event globals
SDL_Event Event;

//input modes
int text_entry_mode;

int numkeys;
Uint8* keystate;

int _init_input() {
_set_text_entry_mode(0); //not used right now
keystate = SDL_GetKeyState(&numkeys); ///returns pointer; only needs to be done once
return 0;
}

int _set_text_entry_mode(int n) {
    if((n != 0) | (n != 1)) { text_entry_mode = n; } else { printf("input.c, _set_text_entry_mode error: mode invalid \n"); }
    return 0;
}

int _get_key_state(key_state_func key_state_cb) {
    SDL_PumpEvents();
    //keystate = SDL_GetKeyState(numkeys);

    _key_state_callback(key_state_cb, keystate, numkeys);
    if(keystate[96] != 0){
    int x;
    for(x=0; x<numkeys; x++) {
        if(keystate[x] != 0) { printf("%i='%c' ", x, (char)x);}
        }
    printf("\n", x);
    }
    //if ( keystate[SDLK_UP] ) {printf( "Up Key" );}
    return 0;
}

int _process_events(mouse_event_func mouse_event_cb, mouse_motion_func mouse_motion_cb, key_event_func keyboard_event_cb, key_text_event_func keyboard_text_event_cb) {
    while(SDL_PollEvent( &Event )) { //returns 0 if no event
    //SDL_PumpEvents();

    if( (Event.type == SDL_MOUSEBUTTONDOWN) || (Event.type == SDL_MOUSEBUTTONUP)) {
         MouseEvent me;
        me.x = Event.motion.x;
        me.y = Event.motion.y;
        me.button = Event.button.button;
        me.state = Event.button.state; //up or down
        _mouse_event_callback(mouse_event_cb, me);
    }

    switch( Event.type )
    {
        case SDL_QUIT:
            printf( "quit" );
            break;

        case SDL_KEYDOWN:
            ///text event
            //printf("%c\n", getUnicodeValue(Event.key.keysym)); //This is for typing
            SDL_EnableUNICODE( SDL_ENABLE );
            if(Event.key.keysym.unicode != 0) {
            _key_event_callback(keyboard_event_cb, getUnicodeValue(Event.key.keysym));
            }
            _key_text_event_callback(keyboard_text_event_cb, getUnicodeValue(Event.key.keysym), SDL_GetKeyName(Event.key.keysym.sym));
            SDL_EnableUNICODE( SDL_DISABLE );
            //SDL_GetKeyName(Event.key));

            break;

        case SDL_MOUSEMOTION:
        {
            MouseMotion ms;
            ms.x = Event.motion.x;
            ms.y = Event.motion.y;
            ms.dx = Event.motion.xrel;
            ms.dy = Event.motion.yrel;
            _mouse_motion_callback(mouse_motion_cb, ms);
            //printf("Current mouse position is: (%d, %d)\n", Event.motion.x, Event.motion.y);
            break;
        }
        break;

        default: break;
    }
}

//SDL_EnableUNICODE( SDL_DISABLE );
}

int poll_events(void) {
    //mouse events
    //system events
    //keyboard events
    return 0;
}
/// Call Backs ///

int _key_state_callback(key_state_func user_func, Uint8* keystate, int numkeys) {
    user_func(keystate, numkeys);
    return 0;
}

int _key_event_callback(key_event_func user_func, char key) {
    user_func(key);
    return 0;
}

int _key_text_event_callback(key_text_event_func user_func, char key, char* key_name) {
    user_func(key, key_name);
    return 0;
}

int _mouse_motion_callback(mouse_motion_func user_func, MouseMotion ms) {
    user_func(ms);
    return 0;
}

int _mouse_event_callback(mouse_event_func user_func, MouseEvent me) {
    user_func(me);
    return 0;
}

/// End Call Backs ///


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

