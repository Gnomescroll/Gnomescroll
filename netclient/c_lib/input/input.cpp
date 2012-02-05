#include "./input.hpp"

#include <c_lib/SDL/SDL_functions.h>
#include <c_lib/input/handlers.hpp>

char getUnicodeValue(SDL_keysym keysym);

static SDL_Event Event;

static int numkeys;
static Uint8* keystate;

int init_input() {
    keystate = SDL_GetKeyState(&numkeys); ///returns pointer; only needs to be done once
    SDL_EnableUNICODE( SDL_ENABLE );
    return 0;
}

int _get_key_state() {
    SDL_PumpEvents();

    key_state_handler(keystate, numkeys);
    if(keystate['`'] != 0)
    {
        int x;
        for(x=0; x<numkeys; x++)
        {
            if(keystate[x] != 0) printf("%i='%c' ", x, (char)x);
        }
        printf("\n");
    }

    return 0;
}

/*
If you want to know what character the user entered (as opposed to what key), try SDL_EnableUNICODE(1). event.keysym.unicode will now contain the (Uint16) character. You can usually simply cast it to a char and you'll have what you want.
One thing to keep in mind is that the unicode field will only be filled on keydown, not keyup.
You should also disable the unicode translation as soon as you're finished with it because it caused extra overhead.

--for coping with shifted keys (e.g, ? = shift + /)
*/

int _process_events(key_event_func keyboard_event_cb, key_text_event_func keyboard_text_event_cb)
{
    int t; //temp

    if (input_state.mouse_bound) {
        SDL_ShowCursor(0);
        SDL_WM_GrabInput(SDL_GRAB_ON);
    } else {
        SDL_ShowCursor(1);
        SDL_WM_GrabInput(SDL_GRAB_OFF);       
    }
    
    while(SDL_PollEvent( &Event )) { //returns 0 if no event

        int event_state = 0;
        char* key_name;

        switch( Event.type )
        {
            case SDL_QUIT:
                quit_event_handler(&Event);
                break;

            case SDL_KEYDOWN:

                key_down_handler(&Event);
                
                t = getUnicodeValue(Event.key.keysym);
                if(t==0) t= Event.key.keysym.sym;
                event_state = 1;
                key_name = SDL_GetKeyName(Event.key.keysym.sym);
                _key_text_event_callback(keyboard_text_event_cb, t, key_name, event_state);
                break;

            case SDL_KEYUP:

                key_up_handler(&Event);

                t = getUnicodeValue(Event.key.keysym);
                if(t==0) t= Event.key.keysym.sym;
                event_state = 0;
                key_name = SDL_GetKeyName(Event.key.keysym.sym);
                _key_text_event_callback(keyboard_text_event_cb, t, SDL_GetKeyName(Event.key.keysym.sym), event_state);
                break;

            case SDL_MOUSEMOTION:
                mouse_motion_handler(&Event);
                break;

            case SDL_MOUSEBUTTONDOWN:
                mouse_button_down_handler(&Event);
                break;
                
            case SDL_MOUSEBUTTONUP:
                mouse_button_up_handler(&Event);
                break;

            default: break;
        }
    }

    return 0;
}

/* Call Backs */

int _key_event_callback(key_event_func user_func, char key) {
    user_func(key);
    return 0;
}

int _key_text_event_callback(key_text_event_func user_func, char key, char* key_name, int event_state) {
    user_func(key, key_name, event_state);
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
static float mouse_input_buffer_timestamps[MOUSE_INPUT_BUFFER_SIZE];
static int mouse_buffer_index = 0;
static struct MouseMotionAverage mm = {0.0f, 0.0f};

#define INITIAL_MOUSE_WEIGHT 1.0f

static inline float _mouse_weight(float t) {
    return INITIAL_MOUSE_WEIGHT * pow(MOUSE_BUFFER_DECAY, t/TICK_DURATION);
}

#define MOUSE_AXIS_AVERAGE(AXIS) \
static inline float mouse_axis_average_##AXIS() { \
    float total = 0.0f; \
    float divisor = 0.0f; \
    float t = 0.0f;\
\
    int i, index; \
    float weight;\
\
    for (i=0; i<MOUSE_INPUT_BUFFER_SIZE; i++) { \
        index = (MOUSE_INPUT_BUFFER_SIZE + mouse_buffer_index - i) % MOUSE_INPUT_BUFFER_SIZE; \
\
        weight = _mouse_weight(t);\
        divisor += weight; \
\
        total += mouse_input_buffer_##AXIS[index] * weight; \
\
        t += mouse_input_buffer_timestamps[index];\
    } \
\
    total /= divisor; \
\
    return total; \
}

MOUSE_AXIS_AVERAGE(x)
MOUSE_AXIS_AVERAGE(y)

static inline void calculate_mouse_state(int t) { // t is time since last tick

    SDL_GetRelativeMouseState(&mouse_input_buffer_x[mouse_buffer_index], &mouse_input_buffer_y[mouse_buffer_index]);

    mouse_input_buffer_timestamps[mouse_buffer_index] = (float)t;
    mm.x = mouse_axis_average_x();
    mm.y = mouse_axis_average_y();

    mouse_buffer_index++;
    mouse_buffer_index %= MOUSE_INPUT_BUFFER_SIZE;
}

struct MouseMotionAverage* get_mouse_render_state(int t) {
    calculate_mouse_state(t);
    return &mm;
}


