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

#include <c_lib/time/physics_timer.h>

//_GET_MS_TIME();

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

    /*
        static int otime = _GET_MS_TIME();
        int one = 0;
        static int event_polls = 0;
        static int counter = 0;
        if( _GET_MS_TIME() -otime > 1000)
        {
            one = 1;
            otime = _GET_MS_TIME();

            //printf("%i events, %i event polls \n", counter, event_polls);
            counter = 0; //reset
            event_polls =0;
        }
        event_polls++;
    */
      
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
                //counter++;
                //printf( "dx,dy= %i %i \n", Event.motion.xrel, Event.motion.yrel);
            
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

/*
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
*/

//    static float vx = 0.0;
//    static float vy = 0.0;

static float vx = 0.0;
static float vy = 0.0;


//    For Zoomed in, use






struct MOUSE_MOVEMENT
{
    long time;  
    int dx;
    int dy;
};

/*
static float x_sensitivity = 1 / (2*3.1415*500);
static float y_sensitivity = 1 / (2*3.1415*500);
static const float dampening = 0.25;
static float linear_sensitivity = 0.0;
*/

//Example of completely flat response

/*
static float x_sensitivity = 0.0;
static float y_sensitivity = 0.0;

static const float dampening = 0.00;
static float linear_sensitivity = 1.0 / (2*3.1415* 120);
*/


static float x_sensitivity = 1 / (2*3.1415*100000);
static float y_sensitivity = 1 / (2*3.1415*100000);
static const float dampening = 0.8;

static float linear_sensitivity = 0.003f;


static struct MOUSE_MOVEMENT MOUSE_MOVEMENT_ARRAY[1000];
static int MOUSE_MOVEMENT_ARRAY_INDEX = 0;

/*
    Normalize vx,vy by time since last frame!
*/
void apply_camera_physics()
{
    printf("");
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

    long _start_time = LAST_MOUSE_MOVEMENT_TIME; //debug
    
    //if(LAST_MOUSE_MOVEMENT_TIME == current_time) return; //at least once ms must pass

    const float cfactor = 1.0/33.3333;
    float _dampening = pow(dampening, cfactor); // dampening per frame

    int index = 0;
   
    float accum_dx = 0;
    float accum_dy = 0;

    float accum_vx = 0;
    float accum_vy = 0;

    while(LAST_MOUSE_MOVEMENT_TIME <= current_time)
    {
        
        //printf("%i %i \n", LAST_MOUSE_MOVEMENT_TIME, MOUSE_MOVEMENT_ARRAY[index].time);

        while( MOUSE_MOVEMENT_ARRAY[index].time == LAST_MOUSE_MOVEMENT_TIME &&  index < MOUSE_MOVEMENT_ARRAY_INDEX)
        {
            int dx = MOUSE_MOVEMENT_ARRAY[index].dx;
            int dy = MOUSE_MOVEMENT_ARRAY[index].dy;
        

            vx += dx*x_sensitivity;
            vy += dy*y_sensitivity;

            accum_dx += dx*linear_sensitivity;
            accum_dy += dy*linear_sensitivity;

            index++;
        }

        accum_vx += vx;     //newton integration, replace with verlet, lulz
        accum_vy += vy;

        vx *= _dampening;
        vy *= _dampening;

        LAST_MOUSE_MOVEMENT_TIME++;
    }
    LAST_MOUSE_MOVEMENT_TIME--;
    
    //printf("index= %i \n", index);
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
    //printf("vx,vy = %f %f \n", vx, vy);


    if (input_state.input_mode == INPUT_STATE_AGENT)
        agent_camera->pan(accum_vx+accum_dx, accum_vy+accum_dy);
    else
        free_camera->pan(accum_vx+accum_dx, accum_vy+accum_dy);

    MOUSE_MOVEMENT_ARRAY_INDEX = 0;
    //LAST_MOUSE_MOVEMENT_TIME = _GET_MS_TIME();
}

void pan_camera(int delta_tick)
{
    int current_time = _GET_MS_TIME();

    static int otime = _GET_MS_TIME();
    static int counter = 0;
    static int non_zero = 0;
    if( current_time - otime > 1000)
    {
        otime = _GET_MS_TIME();
        //printf("%i mouse polls, %i non_zero \n", counter, non_zero);
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


    //printf("event= %i %i \n", dx,dy);


    //vx += dx*x_sensitivity;
    //vy += dy*y_sensitivity; 
    

    
    //int inv = (input_state.invert_mouse) ? 1 : -1;
    //float _dx
    //float _dy;
    
    //_dx = ((float)(-mm->x) * input_state.sensitivity) / 40000.0f; // magic #
    //_dy = ((float)(inv * mm->y) * input_state.sensitivity) / 40000.0f; // magic #
}
