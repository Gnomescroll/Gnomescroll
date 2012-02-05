#pragma once

struct InputState
{
    bool mouse_bound;
};

extern InputState input_state;

void init_handlers();

// routers
void key_down_handler(SDL_Event* event);
void key_up_handler(SDL_Event* event);
void mouse_button_down_handler(SDL_Event* event);
void mouse_button_up_handler(SDL_Event* event);
void mouse_motion_handler(SDL_Event* event);

// handlers
void toggle_mouse_bind();
