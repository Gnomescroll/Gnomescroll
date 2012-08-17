#pragma once

const int SDL_EVENT_USER_NONE = 0;
const int SDL_EVENT_USER_TRIGGER = 1;

typedef enum InputStateMode
{
    INPUT_STATE_AGENT,
    INPUT_STATE_CAMERA
} InputStateMode;

struct InputState
{
    bool mouse_bound;
    bool rebind_mouse;
    bool help_menu;
    bool scoreboard;
    bool map;
    bool chat;
    bool full_chat;
    bool skeleton_editor;
    bool graphs;
    bool equipped_sprite_adjuster;
    
    bool can_jump;
    bool confirm_quit;
    bool quit;

    bool ignore_mouse_motion;
    int last_mouse_button_down_event_frame;
    int last_mouse_button_up_event_frame;
    
    InputStateMode input_mode;
    InputStateMode camera_mode;

    bool ignore_next_right_click_event;

    // options
    bool invert_mouse;
    float sensitivity;

    bool agent_container;
    bool container_block;
    int container_block_id;

    // views
    bool draw_hud;
    bool vbo_debug;
    bool diagnostics;
    bool admin_controls;

    bool frustum;
};

extern InputState input_state;

void init_input_state();
void update_input_state();

// handlers
void key_down_handler(SDL_Event* event);
void key_up_handler(SDL_Event* event);
void mouse_button_down_handler(SDL_Event* event);
void mouse_button_up_handler(SDL_Event* event);
void mouse_motion_handler(SDL_Event* event);
void quit_event_handler(SDL_Event* event);
void active_event_handler(SDL_Event* event);
void key_state_handler(Uint8 *keystate, int numkeys);

// triggers
void toggle_mouse_bind();
void toggle_help_menu();
void toggle_scoreboard();
void toggle_map();
void toggle_chat();
void toggle_skeleton_editor();
void toggle_equipped_sprite_adjuster();

void enable_jump();
void enable_quit();

void toggle_input_mode();
void toggle_camera_mode();

// container toggles
void toggle_agent_container();
void enable_agent_container();
void disable_agent_container();

void enable_container_block(int container_id);
void disable_container_block();

void close_all_containers();

void trigger_keys_held_down();
