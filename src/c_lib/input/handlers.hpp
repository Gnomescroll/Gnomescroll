#pragma once

const int SDL_EVENT_USER_NONE = 0;
const int SDL_EVENT_USER_TRIGGER = 1;

typedef enum
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
    bool equipped_sprite_adjuster;
    bool terminal_is_open;

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

    bool agent_inventory;
    bool container_block;
    int container_block_id;

    // views
    bool draw_hud;
    bool vbo_debug;
    bool diagnostics;
    bool admin_controls;

    // SDL input state
    bool mouse_focus;  // mouse
    bool input_focus;  // keyboard
    bool app_active;    // useless/does not trigger. supposedly minimizing/maximizing window

    // awesomium;
    bool awesomium;

    bool error_message;

    // freezes frustum (probably not used anymore)
    bool frustum;

    // login mode is when we are waiting for login
    bool login_mode;

    bool login_window;
    bool settings_window;
};

extern InputState input_state;

void init_input_state();
void update_input_state();

bool mouse_unlocked_for_ui_element();

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
void disable_chat();
void enable_chat();
void toggle_skeleton_editor();
void toggle_equipped_sprite_adjuster();

void enable_jump();
void enable_quit();

void toggle_input_mode();
void toggle_camera_mode();

// container toggles
void toggle_agent_inventory();
void enable_agent_inventory();
void disable_agent_inventory();

void enable_container_block(ItemContainerID container_id);
void disable_container_block();

void disable_login_window();
void enable_login_window();

void toggle_settings_window();
void enable_settings_window();
void disable_settings_window();

void update_awesomium_window_state();
void disable_awesomium_windows();

void close_all_containers();

void set_mouse_bind(bool bind)
{
    input_state.mouse_bound = bind;
    //printf("Set mouse bind: %d\n", bind);
}

void set_mouse_rebind(bool rebind)
{
    input_state.rebind_mouse = rebind;
    //printf("Set mouse rebind: %d\n", rebind);
}

void trigger_keys_held_down();

namespace Input
{   // TODO -- namespace everything
void begin_login_mode()
{
    //printf("Begin login mode 1. mouse_bound: %d; rebind_mouse: %d\n", input_state.mouse_bound, input_state.rebind_mouse);

    input_state.login_mode = true;
    enable_login_window();

    //printf("Begin login mode 2. mouse_bound: %d; rebind_mouse: %d\n", input_state.mouse_bound, input_state.rebind_mouse);
}

void end_login_mode()
{
    //printf("End login mode 1. mouse_bound: %d; rebind_mouse: %d\n", input_state.mouse_bound, input_state.rebind_mouse);

    disable_login_window();
    input_state.login_mode = false;

    //printf("End login mode 2. mouse_bound: %d; rebind_mouse: %d\n", input_state.mouse_bound, input_state.rebind_mouse);

    // stupid hack
    #if PRODUCTION
    set_mouse_bind(true);
    set_mouse_rebind(true);
    #else
    set_mouse_bind(false);
    set_mouse_rebind(false);
    #endif

    //printf("End login mode 3. mouse_bound: %d; rebind_mouse: %d\n", input_state.mouse_bound, input_state.rebind_mouse);
}

}   // Input
