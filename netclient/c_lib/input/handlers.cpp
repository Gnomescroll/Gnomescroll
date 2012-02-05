#include "handlers.hpp"

// properties struct

InputState input_state;

// key down
void key_down_handler(SDL_Event* event)
{
    if(event->key.keysym.sym == SDLK_HOME)
    {
        printf("Saving Screenshot \n");
        save_screenshot();
    }

     //for Dany0 (azerty testing)
     //while holding n, will show key struct info
    if (keystate['r'] != 0)
    {
        printf("scancode = %d\n", (int)event->key.keysym.scancode);
        printf("keysym = %d\n", (int)event->key.keysym.sym);
        printf("\n");
    }

    switch (event->key.keysym.sym)
    {
        case SDLK_u:
            toggle_mouse_bind();
            break;
        default: break;
    }
}

// key up
void key_up_handler(SDL_Event* event)
{

}

// mouse down
void mouse_button_down_handler(SDL_Event* event)
{

}

// mouse up
void mouse_button_up_handler(SDL_Event* event)
{

}

// mouse motion
void mouse_motion_handler(SDL_Event* event)
{

}

// handlers
void toggle_mouse_bind()
{
    input_state.mouse_bound = (!input_state.mouse_bound);
}

void init_handlers()
{
    // set input_state defaults
    input_state.mouse_bound = false;
}
