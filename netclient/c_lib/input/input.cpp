#include "input.hpp"

#include <SDL/SDL_functions.h>
#include <input/handlers.hpp>

static SDL_Event Event;

static int numkeys;
static Uint8* keystate;

struct MouseMotionAverage 
{
    float x,y;
};

int init_input()
{
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
    //if(keystate['`'] != 0)
    //{
        //for(int x=0; x<numkeys; x++)
            //if(keystate[x] != 0) printf("%i='%c' ", x, (char)x);

        //printf("\n");
    //}

    return 0;
}

void bind_mouse()
{
    if (!input_state.block_container && !input_state.agent_container)
        SDL_ShowCursor(0);
    SDL_WM_GrabInput(SDL_GRAB_ON);
}

void unbind_mouse()
{
    SDL_ShowCursor(1);
    SDL_WM_GrabInput(SDL_GRAB_OFF);
}

//#include <common/lua/lua.hpp>

int process_events()
{
    if (input_state.mouse_bound)
        bind_mouse();
    else
        unbind_mouse();

    if (ItemContainer::container_was_opened())
    {
        enable_block_container();
        // need to ignore remaining right click up event
        input_state.ignore_next_container_right_click_event = true;
    }
    else if (ItemContainer::container_was_closed())
        disable_block_container();

    while(SDL_PollEvent(&Event))
    { //returns 0 if no event
        Event.user.code = SDL_EVENT_USER_NONE;
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
char getUnicodeValue(SDL_keysym keysym) 
{
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
    int current_time = _GET_MS_TIME();

    static int otime = _GET_MS_TIME();
    static int counter = 0;
    static int non_zero = 0;
    if( current_time -otime > 1000)
    {
        otime = _GET_MS_TIME();
        printf("%i mouse polls, %i non_zero \n", counter, non_zero);
        counter = 0; //reset
        non_zero = 0;
    }
    counter++;

    SDL_PumpEvents(); //mouse state does not update unless this is called

    int dx, dy;
    SDL_GetRelativeMouseState(&dx, &dy);

    if(dx == 0 && dy == 0) return;
    non_zero++;

    printf("event= %i %i \n", dx,dy);


    mouse_input_buffer_x[mouse_buffer_index] = dx;
    mouse_input_buffer_y[mouse_buffer_index] = dy;

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

static float vx = 0.0;
static float vy = 0.0;

struct MOUSE_MOVEMENT
{
    long time;  
    int dx;
    int dy;
};

/* DO NOT MODIFY THESE */
static const float DEFAULT_DAMPENING = 0.68f;
static const float DEFAULT_SENSITIVITY_OPTION = 1000.0f;
static const float DEFAULT_LINEAR_SENSITIVITY_COEFFICIENT = 4100;   // higher is less sensitive
static const float DEFAULT_X_SENSITIVITY_COEFFICIENT = 10000;   // higher is less sensitive
static const float DEFAULT_Y_SENSITIVITY_COEFFICIENT = 10000;   // higher is less sensitive
static const float ZOOM_SENSITIVITY_SCALE_FACTOR = 0.50f;
/* end modification warning */

static float x_sensitivity = 1 / (2*3.1415*DEFAULT_X_SENSITIVITY_COEFFICIENT);
static float y_sensitivity = 1 / (2*3.1415*DEFAULT_Y_SENSITIVITY_COEFFICIENT);
static const float dampening = DEFAULT_DAMPENING;
static float linear_sensitivity = 1 / (2 * 3.14159 * DEFAULT_LINEAR_SENSITIVITY_COEFFICIENT* DEFAULT_SENSITIVITY_OPTION);

static const int MOUSE_MOVEMENT_ARRAY_INDEX_MAX = 1000;
static struct MOUSE_MOVEMENT MOUSE_MOVEMENT_ARRAY[MOUSE_MOVEMENT_ARRAY_INDEX_MAX];
static int MOUSE_MOVEMENT_ARRAY_INDEX = 0;

/*
    Normalize vx,vy by time since last frame!
*/
void apply_camera_physics()
{
    static long LAST_MOUSE_MOVEMENT_TIME = _GET_MS_TIME();
    
    long current_time = _GET_MS_TIME();

    if(current_time == LAST_MOUSE_MOVEMENT_TIME)
    {
        return;
        printf("0 apply_camera_physics: Warning: timer error\n");
    }

    if(current_time < LAST_MOUSE_MOVEMENT_TIME)
    {
        printf("1 apply_camera_physics: Warning: ERROR!! timer error\n");
    }

    //long _start_time = LAST_MOUSE_MOVEMENT_TIME; //debug
    
    const float cfactor = 1.0/33.3333;
    float _dampening = pow(dampening, cfactor); // dampening per frame

    int index = 0;
   
    float accum_dx = 0;
    float accum_dy = 0;

    float accum_vx = 0;
    float accum_vy = 0;

    while(LAST_MOUSE_MOVEMENT_TIME <= current_time)
    {
        while( MOUSE_MOVEMENT_ARRAY[index].time == LAST_MOUSE_MOVEMENT_TIME &&  index < MOUSE_MOVEMENT_ARRAY_INDEX)
        {
            int dx = MOUSE_MOVEMENT_ARRAY[index].dx;
            int dy = MOUSE_MOVEMENT_ARRAY[index].dy;

            float dvx,dvy, daccum_dx,daccum_dy;

            dvx = dx*x_sensitivity;
            dvy = dy*y_sensitivity;
            daccum_dx = dx*linear_sensitivity*input_state.sensitivity;
            daccum_dy = dy*linear_sensitivity*input_state.sensitivity;

            if (current_camera != NULL && current_camera->zoomed)
            {
                dvx *= ZOOM_SENSITIVITY_SCALE_FACTOR;
                dvy *= ZOOM_SENSITIVITY_SCALE_FACTOR;
                daccum_dx *= ZOOM_SENSITIVITY_SCALE_FACTOR;
                daccum_dy *= ZOOM_SENSITIVITY_SCALE_FACTOR;
            }

            vx += dvx;
            vy += dvy;
            accum_dx += daccum_dx;
            accum_dy += daccum_dy;

            index++;
        }

        accum_vx += vx;     //newton integration, replace with verlet, lulz
        accum_vy += vy;

        vx *= _dampening;
        vy *= _dampening;

        LAST_MOUSE_MOVEMENT_TIME++;
    }
    LAST_MOUSE_MOVEMENT_TIME--;

    /*
    if(index != MOUSE_MOVEMENT_ARRAY_INDEX)
    {
        printf("apply_camera_physics, error: index= %i MOUSE_MOVEMENT_ARRAY_INDEX= %i \n", index, MOUSE_MOVEMENT_ARRAY_INDEX);

        printf("start_time= %li \n ",_start_time);
        printf("end_time= %li \n ",LAST_MOUSE_MOVEMENT_TIME);
        
        for(int i=0; i<MOUSE_MOVEMENT_ARRAY_INDEX; i++ )
        {
            printf("mouse move %i: at time %li \n", i, MOUSE_MOVEMENT_ARRAY[i].time);
        }
        printf("end error\n");
    }
    //*/

    if (input_state.input_mode == INPUT_STATE_AGENT)
        agent_camera->pan(accum_vx+accum_dx, accum_vy+accum_dy);
    else
        free_camera->pan(accum_vx+accum_dx, accum_vy+accum_dy);

    MOUSE_MOVEMENT_ARRAY_INDEX = 0;
}

void poll_mouse()
{
    if (input_state.agent_container || input_state.block_container) return;
    if (input_state.ignore_mouse_motion)
    {
        // flush mouse buffer
        MOUSE_MOVEMENT_ARRAY[MOUSE_MOVEMENT_ARRAY_INDEX].dx = 0;
        MOUSE_MOVEMENT_ARRAY[MOUSE_MOVEMENT_ARRAY_INDEX].dy = 0;
        MOUSE_MOVEMENT_ARRAY_INDEX++;
        return;
    }
    
    int current_time = _GET_MS_TIME();

    static int otime = _GET_MS_TIME();
    static int counter = 0;
    static int non_zero = 0;
    if( current_time - otime > 1000)
    {
        otime = _GET_MS_TIME();
        counter = 0; //reset
        non_zero = 0;
    }
    counter++;

    SDL_PumpEvents(); //mouse state does not update unless this is called
    int dx, dy;
    SDL_GetRelativeMouseState(&dx, &dy);

    if(dx == 0 && dy == 0) return;
    non_zero++;

    dx *= -1;
    dy *=  input_state.invert_mouse ? 1 : -1;

    struct MOUSE_MOVEMENT m;

    m.time = _GET_MS_TIME();
    m.dx = dx;
    m.dy = dy;

    MOUSE_MOVEMENT_ARRAY[MOUSE_MOVEMENT_ARRAY_INDEX] = m;
    MOUSE_MOVEMENT_ARRAY_INDEX++;

    //_dx = ((float)(-mm->x) * input_state.sensitivity) / 40000.0f; // magic #
    //_dy = ((float)(inv * mm->y) * input_state.sensitivity) / 40000.0f; // magic #
}
