#include "input.hpp"

#include <SDL/SDL_functions.hpp>
#include <input/handlers.hpp>
#include <SDL/awesomium/_interface.hpp>

static int numkeys = 0;
static Uint8* keystate = NULL;

struct MouseMotionAverage 
{
    float x,y;
};

// forward decl
void init_mouse();
void teardown_mouse();

int init_input()
{
    static int inited = 0;
    GS_ASSERT(inited == 0);
    if (inited++) return 1;

    keystate = SDL_GetKeyState(&numkeys);
    SDL_EnableUNICODE(SDL_ENABLE);
    init_mouse();
    init_input_state();

    return 0;
}

void teardown_input()
{
    teardown_mouse();
}

int get_key_state()
{
    SDL_PumpEvents();

    //for (int i=0; i<numkeys; i++) if (keystate[i]) printf("%d ", i);
    //for (int i=0; i<numkeys; i++) if (keystate[i]) { printf("\n"); break; }

    key_state_handler(keystate, numkeys);

    return 0;
}

void bind_mouse()
{
    SDL_ShowCursor(SDL_DISABLE);
    SDL_WM_GrabInput(SDL_GRAB_ON);
}

void unbind_mouse()
{
    SDL_ShowCursor(SDL_ENABLE);
    SDL_WM_GrabInput(SDL_GRAB_OFF);
}

int process_events()
{
    if (input_state.mouse_bound)
        bind_mouse();
    else
        unbind_mouse();

    ItemContainerID container_id = NULL_CONTAINER;
    if (ItemContainer::container_block_was_closed())
    {
        disable_container_block();
        input_state.ignore_next_right_click_event = false;
    }
    if (ItemContainer::container_block_was_opened(&container_id))
    {
        enable_container_block(container_id);
        input_state.ignore_next_right_click_event = true;
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    { //returns 0 if no event
        event.user.code = SDL_EVENT_USER_NONE;

        switch (event.type)
        {
            case SDL_QUIT:
                quit_event_handler(&event);
                break;

            case SDL_ACTIVEEVENT:
                active_event_handler(&event);
                break;

            case SDL_KEYDOWN:
                key_down_handler(&event);
                break;

            case SDL_KEYUP:
                key_up_handler(&event);
                break;

            case SDL_MOUSEMOTION:
                mouse_motion_handler(&event);
                break;

            case SDL_MOUSEBUTTONDOWN:
                mouse_button_down_handler(&event);
                break;
                
            case SDL_MOUSEBUTTONUP:
                mouse_button_up_handler(&event);
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
const int MOUSE_INPUT_BUFFER_SIZE = 32;
const float MOUSE_BUFFER_DECAY = 0.70f;

static int mouse_input_buffer_y[MOUSE_INPUT_BUFFER_SIZE];
static int mouse_input_buffer_x[MOUSE_INPUT_BUFFER_SIZE];
static float mouse_input_buffer_timestamps[MOUSE_INPUT_BUFFER_SIZE];
static int mouse_buffer_index = 0;

static struct MouseMotionAverage mm = {0.0f, 0.0f};

#define INITIAL_MOUSE_WEIGHT 1.0f

static inline float _mouse_weight(float t)
{
    return INITIAL_MOUSE_WEIGHT * powf(MOUSE_BUFFER_DECAY, t/TICK_DURATION);
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
    if (current_time -otime > 1000)
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

    if (dx == 0 && dy == 0) return;
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

static float vx = 0.0f;
static float vy = 0.0f;

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

static float x_sensitivity = 1 / (2*3.1415f*DEFAULT_X_SENSITIVITY_COEFFICIENT);
static float y_sensitivity = 1 / (2*3.1415f*DEFAULT_Y_SENSITIVITY_COEFFICIENT);
static const float dampening = DEFAULT_DAMPENING;
static float linear_sensitivity = 1 / (2 * 3.14159f * DEFAULT_LINEAR_SENSITIVITY_COEFFICIENT* DEFAULT_SENSITIVITY_OPTION);

static const unsigned int MOUSE_MOVEMENT_ARRAY_INDEX_MAX = 1000;
static struct MOUSE_MOVEMENT* MOUSE_MOVEMENT_ARRAY = NULL;
static unsigned int MOUSE_MOVEMENT_ARRAY_INDEX = 0;

void init_mouse()
{
    GS_ASSERT(MOUSE_MOVEMENT_ARRAY == NULL);
    if (MOUSE_MOVEMENT_ARRAY != NULL) return;
    MOUSE_MOVEMENT_ARRAY = (struct MOUSE_MOVEMENT*)calloc(MOUSE_MOVEMENT_ARRAY_INDEX_MAX, sizeof(struct MOUSE_MOVEMENT));
}

void teardown_mouse()
{
    if (MOUSE_MOVEMENT_ARRAY != NULL) free(MOUSE_MOVEMENT_ARRAY);
}

/*
    Normalize vx,vy by time since last frame!
*/
void apply_camera_physics()
{
    GS_ASSERT(MOUSE_MOVEMENT_ARRAY != NULL);
    if (MOUSE_MOVEMENT_ARRAY == NULL) return;

    static long LAST_MOUSE_MOVEMENT_TIME = _GET_MS_TIME();
    long current_time = _GET_MS_TIME();
    if (current_time == LAST_MOUSE_MOVEMENT_TIME) return;

    GS_ASSERT(current_time > LAST_MOUSE_MOVEMENT_TIME);
    if (current_time < LAST_MOUSE_MOVEMENT_TIME) return;
    
    const float cfactor = 1.0f/33.3333f;
    float _dampening = powf(dampening, cfactor); // dampening per frame

    unsigned int index = 0;
   
    float accum_dx = 0;
    float accum_dy = 0;

    float accum_vx = 0;
    float accum_vy = 0;

    while (LAST_MOUSE_MOVEMENT_TIME <= current_time)
    {
        while (MOUSE_MOVEMENT_ARRAY[index].time == LAST_MOUSE_MOVEMENT_TIME &&  index < MOUSE_MOVEMENT_ARRAY_INDEX)
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
    if (index != MOUSE_MOVEMENT_ARRAY_INDEX)
    {
        printf("apply_camera_physics, error: index= %d MOUSE_MOVEMENT_ARRAY_INDEX= %i \n", index, MOUSE_MOVEMENT_ARRAY_INDEX);

        printf("start_time= %li \n ",_start_time);
        printf("end_time= %li \n ",LAST_MOUSE_MOVEMENT_TIME);
        
        for (unsigned int i=0; i<MOUSE_MOVEMENT_ARRAY_INDEX; i++)
        {
            printf("mouse move %d: at time %li \n", i, MOUSE_MOVEMENT_ARRAY[i].time);
        }
        printf("end error\n");
    }
    //*/

    if (input_state.login_mode)
        current_camera->pan(accum_vx+accum_dx, accum_vy+accum_dy);
    else
    {
        if (input_state.input_mode == INPUT_STATE_AGENT)
            agent_camera->pan(accum_vx+accum_dx, accum_vy+accum_dy);
        else
            free_camera->pan(accum_vx+accum_dx, accum_vy+accum_dy);
    }

    MOUSE_MOVEMENT_ARRAY_INDEX = 0;
}

void poll_mouse()
{
    GS_ASSERT(MOUSE_MOVEMENT_ARRAY != NULL);
    if (MOUSE_MOVEMENT_ARRAY == NULL) return;
    if (!input_state.login_mode && mouse_unlocked_for_ui_element()) return;

    if (input_state.ignore_mouse_motion)
    {   // flush mouse buffer
        MOUSE_MOVEMENT_ARRAY[MOUSE_MOVEMENT_ARRAY_INDEX].dx = 0;
        MOUSE_MOVEMENT_ARRAY[MOUSE_MOVEMENT_ARRAY_INDEX].dy = 0;
        MOUSE_MOVEMENT_ARRAY_INDEX++;
        MOUSE_MOVEMENT_ARRAY_INDEX %= MOUSE_MOVEMENT_ARRAY_INDEX_MAX;
        return;
    }
    
    int current_time = _GET_MS_TIME();

    static int otime = _GET_MS_TIME();
    static int counter = 0;
    static int non_zero = 0;
    if (current_time - otime > 1000)
    {
        otime = _GET_MS_TIME();
        counter = 0; //reset
        non_zero = 0;
    }
    counter++;

    SDL_PumpEvents(); //mouse state does not update unless this is called
    int dx, dy;
    SDL_GetRelativeMouseState(&dx, &dy);

    if (dx == 0 && dy == 0) return;
    non_zero++;

    dx *= -1;
    dy *=  input_state.invert_mouse ? -1 : 1;

    struct MOUSE_MOVEMENT m;

    m.time = _GET_MS_TIME();
    m.dx = dx;
    m.dy = dy;

    MOUSE_MOVEMENT_ARRAY[MOUSE_MOVEMENT_ARRAY_INDEX] = m;
    MOUSE_MOVEMENT_ARRAY_INDEX++;
    MOUSE_MOVEMENT_ARRAY_INDEX %= MOUSE_MOVEMENT_ARRAY_INDEX_MAX;
}
