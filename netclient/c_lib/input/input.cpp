#include "./input.hpp"

#include <c_lib/SDL/SDL_functions.h>
#include <c_lib/input/handlers.hpp>

static SDL_Event Event;

static int numkeys;
static Uint8* keystate;

int init_input() {
    static int inited = 0;
    if (inited) return 1;
    keystate = SDL_GetKeyState(&numkeys);
    SDL_EnableUNICODE(SDL_ENABLE);
    inited++;
    return 0;
}

int get_key_state() {
    SDL_PumpEvents();

    key_state_handler(keystate, numkeys);
    if(keystate['`'] != 0)
    {
        for(int x=0; x<numkeys; x++)
            if(keystate[x] != 0) printf("%i='%c' ", x, (char)x);

        printf("\n");
    }

    return 0;
}

int process_events()
{
    if (input_state.mouse_bound)
    {
        SDL_ShowCursor(0);
        SDL_WM_GrabInput(SDL_GRAB_ON);
    }
    else
    {
        SDL_ShowCursor(1);
        SDL_WM_GrabInput(SDL_GRAB_OFF);
    }

    
    while(SDL_PollEvent( &Event )) { //returns 0 if no event

        switch( Event.type )
        {
            case SDL_QUIT:
                quit_event_handler(&Event);
                break;

            case SDL_ACTIVEEVENT:
                active_event_handler(&Event);
                break;

            case SDL_KEYDOWN:
                key_down_handler(&Event);
                break;

            case SDL_KEYUP:
                key_up_handler(&Event);
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

// Taken from somewhere on the internet:
char getUnicodeValue(SDL_keysym keysym) {
    // magic numbers courtesy of SDL docs :)
    const int INTERNATIONAL_MASK = 0xFF80, UNICODE_MASK = 0x7F;
    int uni = keysym.unicode;
    if (uni == 0) // not translatable key (like up or down arrows)
        return 0;

    if ((uni & INTERNATIONAL_MASK) == 0)
    {
        if (SDL_GetModState() & KMOD_SHIFT)
            return (char)(toupper(uni & UNICODE_MASK));
        else
            return (char)(uni & UNICODE_MASK);
    }
    else // we have a funky international character. one we can't read :(
        return '?';
}

/* Separate Mouse querying for physics-independent camera */
static int mouse_input_buffer_y[MOUSE_INPUT_BUFFER_SIZE];
static int mouse_input_buffer_x[MOUSE_INPUT_BUFFER_SIZE];
static float mouse_input_buffer_timestamps[MOUSE_INPUT_BUFFER_SIZE];
static int mouse_buffer_index = 0;
static struct MouseMotionAverage mm = {0.0f, 0.0f};

#define INITIAL_MOUSE_WEIGHT 1.0f

static inline float _mouse_weight(float t)
{
    return INITIAL_MOUSE_WEIGHT * pow(MOUSE_BUFFER_DECAY, t/TICK_DURATION);
}

static inline float mouse_axis_average(int* buffer)
{ 
    float total = 0.0f; 
    float divisor = 0.0f; 
    float t = 0.0f;

    int i, index; 
    float weight;

    for (i=0; i<MOUSE_INPUT_BUFFER_SIZE; i++) { 
        index = (MOUSE_INPUT_BUFFER_SIZE + mouse_buffer_index - i) % MOUSE_INPUT_BUFFER_SIZE; 

        weight = _mouse_weight(t);
        divisor += weight; 

        total += buffer[index] * weight; 

        t += mouse_input_buffer_timestamps[index];
    } 

    total /= divisor; 

    return total; 
}

static inline void calculate_mouse_state(int t) // t is time since last tick
{
    SDL_GetRelativeMouseState(&mouse_input_buffer_x[mouse_buffer_index], &mouse_input_buffer_y[mouse_buffer_index]);

    mouse_input_buffer_timestamps[mouse_buffer_index] = (float)t;
    mm.x = mouse_axis_average(mouse_input_buffer_x);
    mm.y = mouse_axis_average(mouse_input_buffer_y);

    mouse_buffer_index++;
    mouse_buffer_index %= MOUSE_INPUT_BUFFER_SIZE;
}

struct MouseMotionAverage* get_mouse_render_state(int t)
{
    calculate_mouse_state(t);
    return &mm;
}

void pan_camera(int delta_tick)
{
    struct MouseMotionAverage* mm;
    mm = get_mouse_render_state(delta_tick);

    int inv = (input_state.invert_mouse) ? 1 : -1;
    float dx,dy;
    dx = ((float)(-mm->x) * input_state.sensitivity) / 40000.0f; // magic #
    dy = ((float)(inv * mm->y) * input_state.sensitivity) / 40000.0f; // magic #

    if (input_state.input_mode == INPUT_STATE_AGENT)
        agent_camera->pan(dx,dy);
    else
        free_camera->pan(dx,dy);
}
