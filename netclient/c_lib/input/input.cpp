#include "./input.hpp"

#include <c_lib/SDL/camera.h>

static SDL_Event Event;

//input modes
static int text_entry_mode;

static int numkeys;
static Uint8* keystate;

int _init_input() {
    keystate = SDL_GetKeyState(&numkeys); ///returns pointer; only needs to be done once
    SDL_EnableUNICODE( SDL_ENABLE );
    return 0;
}

//deprecate
int _set_text_entry_mode(int n) {
    if((n != 0) || (n != 1)) {
        text_entry_mode = n;
    } else {
        printf("input.c, _set_text_entry_mode error: mode invalid \n");
    }
    return 0;
}

/*
    mouse bound at startup?
*/
static int MOUSE_BOUND = 0;

int _toggle_mouse_bind() {
    MOUSE_BOUND = (MOUSE_BOUND + 1) %2;
    return MOUSE_BOUND;
}

int _get_key_state(key_state_func key_state_cb) {
    SDL_PumpEvents();

    _key_state_callback(key_state_cb, keystate, numkeys);  ///this is only line that does anything
    if(keystate[96] != 0){
        int x;
        for(x=0; x<numkeys; x++) {
            if(keystate[x] != 0) { printf("%i='%c' ", x, (char)x);}
            }
        printf("\n");
    }
    return 0;
}

int _process_events(mouse_event_func mouse_event_cb, mouse_motion_func mouse_motion_cb, key_event_func keyboard_event_cb, key_text_event_func keyboard_text_event_cb, quit_event_func quit_event_cb) {
    int t; //temp

    if(MOUSE_BOUND) {
        SDL_ShowCursor(0);
        SDL_WM_GrabInput(SDL_GRAB_ON);
    } else {
        SDL_ShowCursor(1);
        SDL_WM_GrabInput(SDL_GRAB_OFF);       
    }
    while(SDL_PollEvent( &Event )) { //returns 0 if no event

        if( (Event.type == SDL_MOUSEBUTTONDOWN) || (Event.type == SDL_MOUSEBUTTONUP)) {
             MouseEvent me;
            me.x = Event.motion.x;
            me.y = Event.motion.y;
            me.button = Event.button.button;
            me.state = Event.button.state; //up or down
            _mouse_event_callback(mouse_event_cb, me);
        }

        int event_state = 0;
        char* key_name;

        switch( Event.type )
        {
            case SDL_QUIT:
                _quit_event_callback(quit_event_cb);
                break;

            case SDL_KEYDOWN:
                t = getUnicodeValue(Event.key.keysym);
                if(t==0) t= Event.key.keysym.sym;
                event_state = 1;
                key_name = SDL_GetKeyName(Event.key.keysym.sym);
                _key_text_event_callback(keyboard_text_event_cb, t, key_name, event_state);
                break;

            case SDL_KEYUP:
                t = getUnicodeValue(Event.key.keysym);
                if(t==0) t= Event.key.keysym.sym;
                event_state = 0;
                key_name = SDL_GetKeyName(Event.key.keysym.sym);
                _key_text_event_callback(keyboard_text_event_cb, t, SDL_GetKeyName(Event.key.keysym.sym), event_state);
                break;

            case SDL_MOUSEMOTION:
                MouseMotion ms;
                ms.x = Event.motion.x;
                ms.y = Event.motion.y;
                ms.dx = Event.motion.xrel;
                ms.dy = Event.motion.yrel;
                ms.button = Event.motion.state;
                _mouse_motion_callback(mouse_motion_cb, ms);
                break;

            default: break;
        }
    }

    return 0;
}

int poll_events(void) {
    return 0;
}

/* Call Backs */

int _key_state_callback(key_state_func user_func, Uint8* keystate, int numkeys) {
    user_func(keystate, numkeys);
    return 0;
}

int _key_event_callback(key_event_func user_func, char key) {
    user_func(key);
    return 0;
}

int _key_text_event_callback(key_text_event_func user_func, char key, char* key_name, int event_state) {
    user_func(key, key_name, event_state);
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

int _quit_event_callback(quit_event_func user_func) {
    user_func();
    return 0;
}

/* End Call Backs */


char getUnicodeValue(SDL_keysym keysym ) {
    // magic numbers courtesy of SDL docs :)
    const int INTERNATIONAL_MASK = 0xFF80, UNICODE_MASK = 0x7F;
    int uni = keysym.unicode;
    if( uni == 0 ) // not translatable key (like up or down arrows)
    {
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


/* Separate Mouse querying for physics-independent camera */
static int mouse_input_buffer_y[MOUSE_INPUT_BUFFER_SIZE];
static int mouse_input_buffer_x[MOUSE_INPUT_BUFFER_SIZE];
static int mouse_buffer_index = 0;
static struct MouseMotionAverage mm = {0.0f, 0.0f};

#define MOUSE_AXIS_AVERAGE(AXIS) \
    static inline float mouse_axis_average_##AXIS() { \
        float ttl = 0.0f; \
        float divisor = 0.0f; \
 \
        int i,  index; \
        float decay = 1.0f; \
        for (i=0; i<MOUSE_INPUT_BUFFER_SIZE; i++) { \
            index = (MOUSE_INPUT_BUFFER_SIZE + mouse_buffer_index - i) % MOUSE_INPUT_BUFFER_SIZE; \
            ttl += mouse_input_buffer_##AXIS[index] * decay; \
            divisor += decay; \
            decay *= MOUSE_BUFFER_DECAY; \
        } \
 \
        ttl /= divisor; \
 \
        return ttl; \
    }

MOUSE_AXIS_AVERAGE(x)
MOUSE_AXIS_AVERAGE(y)

static inline void calculate_mouse_state() {

    mouse_buffer_index++;
    mouse_buffer_index %= MOUSE_INPUT_BUFFER_SIZE;

    SDL_GetRelativeMouseState(&mouse_input_buffer_x[mouse_buffer_index], &mouse_input_buffer_y[mouse_buffer_index]);

    //if (mouse_buffer_index < 0) mouse_buffer_index += MOUSE_INPUT_BUFFER_SIZE;

    mm.x = mouse_axis_average_x();
    mm.y = mouse_axis_average_y();
}

struct MouseMotionAverage* get_mouse_render_state() {
    calculate_mouse_state();
    return &mm;
}


