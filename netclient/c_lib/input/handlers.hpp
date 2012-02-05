#pragma once

typedef enum InputStateMode
{
    INPUT_STATE_AGENT,
    INPUT_STATE_CAMERA
} InputStateMode;

struct InputState
{
    bool mouse_bound;
    bool help_menu;
    bool inventory;
    bool scoreboard;
    bool map;
    bool chat;
    bool hud;

    bool can_jump;
    bool quit;

    InputStateMode input_mode;
    InputStateMode camera_mode;
};

extern InputState input_state;

void init_handlers();

// handlers
void key_down_handler(SDL_Event* event);
void key_up_handler(SDL_Event* event);
void mouse_button_down_handler(SDL_Event* event);
void mouse_button_up_handler(SDL_Event* event);
void mouse_motion_handler(SDL_Event* event);
void quit_event_handler(SDL_Event* event);
void key_state_handler(Uint8 *keystate, int numkeys);
// triggers
void toggle_mouse_bind();
void toggle_help_menu();
void toggle_inventory();
void toggle_scoreboard();
void toggle_map();
void toggle_chat();

void enable_jump();
void enable_quit();

void toggle_input_mode();
void toggle_camera_mode();
