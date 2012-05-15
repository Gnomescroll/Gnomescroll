#include "handlers.hpp"

#include <math.h>
#include <chat/client.hpp>
#include <hud/hud.hpp>
#include <input/skeleton_editor.hpp>

//toggling graphics settings
#include <t_map/glsl/shader.hpp>

#include <t_hud/_interface.hpp>

InputState input_state;

// triggers
void toggle_mouse_bind()
{
    input_state.mouse_bound = (!input_state.mouse_bound);
}

void toggle_help_menu()
{
    input_state.help_menu = (!input_state.help_menu);
}

static bool rebind_mouse = false;
void toggle_agent_container()
{
    input_state.agent_container = (!input_state.agent_container);
    if (input_state.agent_container)
    {
        t_hud::enable_agent_container_hud();
        Item::open_inventory();
        rebind_mouse = input_state.mouse_bound;
        input_state.mouse_bound = false;
    }
    else
    {
        t_hud::disable_agent_container_hud();
        Item::close_inventory();
        input_state.mouse_bound = rebind_mouse;
        input_state.ignore_mouse_motion = true;
    }
}

void enable_block_container()
{
    if (input_state.block_container) return;
    printf("enable block container\n");
    input_state.block_container = true;
    t_hud::enable_block_container_hud();
    rebind_mouse = input_state.mouse_bound;
    input_state.mouse_bound = false;
}

void disable_block_container()
{
    if (!input_state.block_container) return;
    printf("disable block container\n");
    input_state.block_container = false;
    t_hud::disable_block_container_hud();
    Item::close_container();
    input_state.mouse_bound = rebind_mouse;
    input_state.ignore_mouse_motion = true;
}

void toggle_scoreboard()
{
    input_state.scoreboard = (!input_state.scoreboard);
}

void toggle_map()
{
    input_state.map = (!input_state.map);
}

void toggle_chat()
{
    input_state.chat = (!input_state.chat);
}

void toggle_full_chat()
{
    input_state.full_chat = (!input_state.full_chat);
}

void toggle_hud()
{
    #if PRODUCTION
    Options::hud = true;
    #else
    Options::hud = (!Options::hud);
    #endif
}

void toggle_debug()
{
    input_state.debug = (!input_state.debug);
}

void toggle_graphs()
{
    input_state.graphs = (!input_state.graphs);
}

void enable_jump()
{
    input_state.can_jump = true;
}

void enable_quit()
{   // release mouse
    unbind_mouse();
    input_state.quit = true;
}

void toggle_confirm_quit()
{
    #if PRODUCTION
    input_state.confirm_quit = (!input_state.confirm_quit);
    #else
    enable_quit();
    #endif
}

void toggle_skeleton_editor()
{
    #if PRODUCTION
    input_state.skeleton_editor = false;
    #else
    input_state.skeleton_editor = (!input_state.skeleton_editor);
    if (input_state.skeleton_editor)
        printf("Skeleton editor enabled\n");
    #endif
}

void toggle_input_mode()
{
    switch (input_state.input_mode)
    {
        case INPUT_STATE_AGENT:
            input_state.input_mode = INPUT_STATE_CAMERA;
            break;
        case INPUT_STATE_CAMERA:
            if (ClientState::playerAgent_state.you != NULL)
                input_state.input_mode = INPUT_STATE_AGENT;
            break;
        default: break;
    }
}

void toggle_camera_mode()
{
    switch (input_state.camera_mode)
    {
        case INPUT_STATE_AGENT:
            input_state.camera_mode = INPUT_STATE_CAMERA;
            break;
        case INPUT_STATE_CAMERA:
            if (ClientState::playerAgent_state.you != NULL)
                input_state.camera_mode = INPUT_STATE_AGENT;
            break;
        default: break;
    }
}

void init_handlers()
{   // set input_state defaults

    #if PRODUCTION
    input_state.mouse_bound = true;
    input_state.debug = false;
    input_state.input_mode = INPUT_STATE_AGENT;
    input_state.camera_mode = INPUT_STATE_AGENT;
    input_state.graphs = false;
    #else
    input_state.mouse_bound = false;
    input_state.debug = false;
    input_state.input_mode = INPUT_STATE_CAMERA;
    input_state.camera_mode = INPUT_STATE_CAMERA;
    input_state.graphs = true;
    #endif

    input_state.help_menu = false;
    input_state.agent_container = false;
    input_state.block_container = false;
    input_state.scoreboard = false;
    input_state.map = false;
    input_state.chat = false;
    input_state.full_chat = false;
    //input_state.hud = true;
    input_state.skeleton_editor = false;

    input_state.can_jump = true;
    input_state.quit = false;

    input_state.ignore_mouse_motion = false;
    input_state.last_mouse_button_down_event_frame = -1;
    input_state.last_mouse_button_up_event_frame = -1;
    
    // options
    input_state.invert_mouse = false;
    input_state.sensitivity = 100.0f;
}

// keys that can be held down
const int KEY_HOLD_DELAY = 20;  // in ticks
const int KEY_HOLD_RATE = 2;    // 15 pulses per second

typedef enum {
    kBACKSPACE = 0,
    kLEFT_ARROW,
    kRIGHT_ARROW,
    kUP_ARROW,
    kDOWN_ARROW
} held_down_key;

typedef struct {
    bool pressed;
    int t;
} key_rate_limit;

const int KEYS_HELD_DOWN = 5;
static key_rate_limit keys_held_down[KEYS_HELD_DOWN];
static const SDLKey keys_held_down_map[KEYS_HELD_DOWN] = {
    SDLK_BACKSPACE,
    SDLK_LEFT,
    SDLK_RIGHT,
    SDLK_UP,
    SDLK_DOWN
};

static void update_keys_held_down()
{
    for (int i=0; i<KEYS_HELD_DOWN; i++)
    {
        if (keys_held_down[i].pressed)
            keys_held_down[i].t++;
        else
            keys_held_down[i].t = 0;
    }
}

void trigger_keys_held_down()
{
    update_keys_held_down();
    static SDL_Event event;
    for (int i=0; i<KEYS_HELD_DOWN; i++)
    {
        if (keys_held_down[i].t > KEY_HOLD_DELAY
          && (keys_held_down[i].t - KEY_HOLD_DELAY) % KEY_HOLD_RATE == 0)
        {
            event.user.code = SDL_EVENT_USER_TRIGGER;
            event.key.keysym.sym = keys_held_down_map[i];
            key_down_handler(&event);
        }
    }
}

// options
const float ZOOM_SENSITIVITY_SCALE = 0.7f;
void set_input_options(
    bool invert_mouse,
    float sensitivity
)
{
    input_state.invert_mouse = invert_mouse;
    input_state.sensitivity = sensitivity;
    if (Hud::hud_draw_settings.zoom)
        input_state.sensitivity *= ZOOM_SENSITIVITY_SCALE;
}

/* Chat buffer */

void chat_key_down_handler(SDL_Event* event)
{
    using namespace Hud;
    
    switch (event->key.keysym.sym)
    {
        case SDLK_ESCAPE:
            chat_client->input->clear_buffer();
            toggle_chat();
            return;
        case SDLK_BACKSPACE:
            chat_client->input->remove();
            return;
        case SDLK_RETURN:
            chat_client->submit();
            toggle_chat();
            return;
        case SDLK_LEFT:
            chat_client->input->cursor_left();
            return;
        case SDLK_RIGHT:
            chat_client->input->cursor_right();
            return;
        case SDLK_UP:
            chat_client->input->history_older();
            return;
        case SDLK_DOWN:
            chat_client->input->history_newer();
            return;
        //case SDLK_PAGEUP:
            //if (hud->inited && hud->chat != NULL && hud->chat->inited)
                //hud->chat->page_up();
            //return;
        //case SDLK_PAGEDOWN:
            //if (hud->inited && hud->chat != NULL && hud->chat->inited)
                //hud->chat->page_down();
            //return;
        default: break;
    }
    
    int t = getUnicodeValue(event->key.keysym);
    t = (t) ? t : event->key.keysym.sym;

    if (t < 0 || t > 127)
        return;
    chat_client->input->add((char)t);
}

void chat_key_up_handler(SDL_Event* event){}
void chat_mouse_down_handler(SDL_Event* event){}
void chat_mouse_up_handler(SDL_Event* event){}
void chat_mouse_motion_handler(SDL_Event* event){}

/* Container / HUD */

void container_key_down_handler(SDL_Event* event)
{
    switch (event->key.keysym.sym)
    {
        case SDLK_e:
        case SDLK_ESCAPE:
            if (input_state.block_container) disable_block_container();
            else toggle_agent_container();
            break;

        default: break;
    }
}

void container_key_up_handler(SDL_Event* event)
{
}

void container_mouse_down_handler(SDL_Event* event)
{
    //// check intersection with any slots

    ////SDL_MouseButtonEvent e = event->button;
    ////printf("Button type: %d\n", e.type);
    ////printf("Button which: %d\n", e.which);
    ////printf("Button button: %d\n", e.button);
    ////printf("Button state: %d\n", e.state);
    ////printf("Button x,y: %d,%d\n", e.x, e.y);  // reports 0,0 no matter what the mouse grab state is

    //// gets correct mouse pixels
    //int x,y;
    //SDL_GetMouseState(&x, &y);
    ////printf("GetMouseState x,y: %d,%d\n", x,y);

    //t_hud::ContainerInputEvent container_event;

    //switch (event->button.button)
    //{
        //case SDL_BUTTON_LEFT:
            //container_event = t_hud::left_mouse_down(x,y);
            //Item::mouse_left_click_handler(container_event.container_id, container_event.slot);
            //break;

        //case SDL_BUTTON_RIGHT:
            //container_event = t_hud::right_mouse_down(x,y);
            //Item::mouse_right_click_handler(container_event.container_id, container_event.slot);
            //break;

        //default:
            //return;
    //}
}

void container_mouse_up_handler(SDL_Event* event)
{
    // check intersection with any slots

    //SDL_MouseButtonEvent e = event->button;
    //printf("Button type: %d\n", e.type);
    //printf("Button which: %d\n", e.which);
    //printf("Button button: %d\n", e.button);
    //printf("Button state: %d\n", e.state);
    //printf("Button x,y: %d,%d\n", e.x, e.y);  // reports 0,0 no matter what the mouse grab state is

    // gets correct mouse pixels
    int x,y;
    SDL_GetMouseState(&x, &y);
    //printf("GetMouseState x,y: %d,%d\n", x,y);

    t_hud::ContainerInputEvent container_event;

    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            container_event = t_hud::left_mouse_up(x,y);
            Item::mouse_left_click_handler(container_event.container_id, container_event.slot, container_event.nanite, container_event.craft_output);
            break;

        case SDL_BUTTON_RIGHT:
            container_event = t_hud::right_mouse_up(x,y);
            Item::mouse_right_click_handler(container_event.container_id, container_event.slot, container_event.nanite, container_event.craft_output);
            break;

        default:
            return;
    }
}

void container_mouse_motion_handler(SDL_Event* event)
{
    //SDL_MouseMotionEvent e = event->motion;

    //printf("Motion type: %d\n", e.type);
    //printf("Motion state: %d\n", e.state);
    //printf("Motion x,y: %d,%d\n", e.x, e.y);
    //printf("Motion xrel,yrel: %d,%d\n", e.xrel, e.yrel);

    int x,y;
    SDL_GetMouseState(&x, &y);
    //printf("X,Y %d,%d\n", x, y);

    /* Coordinates start at TOP LEFT */
    t_hud::mouse_motion(x,y);
}


/* Agent */

#include <common/lua/lua.hpp>
//lua_load_map_tiles();

#include <t_mech/state.hpp>

void agent_key_down_handler(SDL_Event* event)
{
    t_hud::ContainerInputEvent container_event;
    switch (event->key.keysym.sym)
    {
        case SDLK_k:
            run_lua_test();
            break;

        //case SDLK_9:
            //t_mech::add_mech();
            //break;

        //case SDLK_0:
            //t_mech::rotate_mech();
            //break;

        case SDLK_r:
            Toolbelt::reload_event();
            break;
        
        case SDLK_LEFT:
            HudCubeSelector::cube_selector.left();
            break;
        case SDLK_RIGHT:
            HudCubeSelector::cube_selector.right();
            break;
        case SDLK_UP:
            HudCubeSelector::cube_selector.up();
            break;
        case SDLK_DOWN:
            HudCubeSelector::cube_selector.down();
            break;

        case SDLK_1:
            container_event = t_hud::select_slot(1);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_2:
            container_event = t_hud::select_slot(2);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_3:
            container_event = t_hud::select_slot(3);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_4:
            container_event = t_hud::select_slot(4);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_5:
            container_event = t_hud::select_slot(5);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_6:
            container_event = t_hud::select_slot(6);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_7:
            container_event = t_hud::select_slot(7);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_8:
            container_event = t_hud::select_slot(8);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_9:
            container_event = t_hud::select_slot(9);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
            
        default: break;
    }
    
}

void agent_key_up_handler(SDL_Event* event)
{
    switch (event->key.keysym.sym)
    {
        case SDLK_SPACE:
            enable_jump();
            break;
            
        default: break;
    }
}

void agent_mouse_down_handler(SDL_Event* event)
{
    if (ClientState::playerAgent_state.you == NULL) return;

    t_hud::ContainerInputEvent container_event;
    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            Toolbelt::left_trigger_down_event();
            break;

        case SDL_BUTTON_RIGHT:
            Toolbelt::right_trigger_down_event();
            break;

        case SDL_BUTTON_MIDDLE:
            // nothing
            break;

        case 4: // scroll up
            container_event = t_hud::scroll_up();
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;

        case 5: // scroll down
            container_event = t_hud::scroll_down();
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;

        default: break;
    }
}

void agent_mouse_up_handler(SDL_Event* event)
{
    if (ClientState::playerAgent_state.you == NULL) return;

    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            Toolbelt::left_trigger_up_event();
            break;

        case SDL_BUTTON_RIGHT:
            Toolbelt::right_trigger_up_event();
            break;

        case SDL_BUTTON_MIDDLE:
            // nothing
            break;

        default: break;
    }
}

void agent_mouse_motion_handler(SDL_Event* event){}

void agent_key_state_handler(Uint8 *keystate, int numkeys,
    char *f, char *b, char *l, char *r,
    char *jet, char *jump, char *crouch, char *boost,
    char *m1, char *m2, char *m3
)
{
    if (keystate['w'])
        *f = 1;
    if (keystate['s'])
        *b = 1;
    if (keystate['a'])
        *l = 1;
    if (keystate['d'])
        *r = 1;
    if (keystate['z'])
        *jet = 1;
    if (keystate[' '])
        *jump = 1;
    if (keystate[306])  // LCTRL
        *crouch = 1;
}


/* Camera */

void camera_key_down_handler(SDL_Event* event)
{
    switch (event->key.keysym.sym)
    {
        case SDLK_p:    // jump to agent camera
            free_camera->copy_state_from(agent_camera);
            break;
        default: break;
    }
}

void camera_key_up_handler(SDL_Event* event){}
void camera_mouse_down_handler(SDL_Event* event)
{
    switch (event->button.button)
    {
        case SDL_BUTTON_RIGHT:
            free_camera->toggle_zoom();
            break;

        default: break;
    }
}

void camera_mouse_up_handler(SDL_Event* event){}
void camera_mouse_motion_handler(SDL_Event* event){}

void camera_key_state_handler(Uint8 *keystate, int numkeys)
{
    const float speed = 0.8f;
    
    if (keystate['w'])
        free_camera->move(speed, 0,0);
    if (keystate['s'])
        free_camera->move(-speed,0,0);
    if (keystate['a'])
        free_camera->move(0, speed, 0);
    if (keystate['d'])
        free_camera->move(0,-speed,0);
    if (keystate['r'])
        free_camera->move(0,0,speed);
    if (keystate['f'])
        free_camera->move(0,0,-speed);
}

/*
 * Reference:
 * http://wiki.libsdl.org/moin.cgi/SDLKeycodeLookup
 */

// Handlers

// key down
void key_down_handler(SDL_Event* event)
{

    if (input_state.skeleton_editor)
    {
        SkeletonEditor::key_down_handler(event);
        return;
    }

    if (input_state.agent_container || input_state.block_container)
        container_key_down_handler(event);
    else if (input_state.chat)
        chat_key_down_handler(event);
    else
    {
        if (input_state.input_mode == INPUT_STATE_AGENT)
            agent_key_down_handler(event);
        else
            camera_key_down_handler(event);

    
        // these should occur for both Agent and Camera
        switch (event->key.keysym.sym)
        {
            case SDLK_b:
                if (input_state.debug)
                    Animations::agent_bleed(
                        ClientState::playerAgent_state.camera_state.x,
                        ClientState::playerAgent_state.camera_state.y,
                        ClientState::playerAgent_state.camera_state.z
                    );
                break;
                
            case SDLK_g:
                if (input_state.debug)
                    toggle_camera_mode();
                break;
                
            case SDLK_h:
                toggle_help_menu();
                break;

            case SDLK_m:
                toggle_map();
                break;

            case SDLK_n:
                if (input_state.confirm_quit)
                    toggle_confirm_quit();
                break;

            case SDLK_o:
                toggle_skeleton_editor();
                break;
            
            case SDLK_t:
                if (input_state.debug)
                    toggle_input_mode();
                else
                {
                    toggle_chat();
                    chat_client->use_global_channel();
                }
                break;
                
            case SDLK_u:
                toggle_mouse_bind();
                break;

            case SDLK_y:
                if (input_state.confirm_quit)
                {
                    enable_quit();
                    break;
                }
                toggle_chat();
                if (!input_state.debug)
                    chat_client->use_team_channel();
                break;

            case SDLK_TAB:
                toggle_scoreboard();
                break;

            case SDLK_RETURN:
                toggle_chat();
                chat_client->use_global_channel();
                break;

            case SDLK_SLASH:
                toggle_hud();
                break;

            case SDLK_LEFTBRACKET:
                if (input_state.debug)
                    ClientState::playerAgent_state.toggle_camera_mode();
                break;

            case SDLK_QUOTE:
                toggle_graphs();
                break;

            case SDLK_e:
                toggle_agent_container();
                break;

            case SDLK_ESCAPE:
                if (NetClient::Server.connected)
                    toggle_confirm_quit();
                else
                    enable_quit();
                break;

            default: break;
        }
    }

    // these should occur for all of Chat, Agent and Camera
    switch (event->key.keysym.sym)
    {
        case SDLK_HOME:
            save_screenshot();
            break;

        case SDLK_F10:
            t_map::toggle_3d_texture_settings();
            break;

        case SDLK_F12:
            toggle_debug();
            break;

        case SDLK_LALT:
        case SDLK_RALT:
            input_state.mouse_bound = false;
            break;

        case SDLK_BACKQUOTE:
            toggle_full_chat();
            break;

        case SDLK_BACKSPACE:
            if (event->user.code != SDL_EVENT_USER_TRIGGER)
                keys_held_down[kBACKSPACE].pressed = true;
            break;
        case SDLK_LEFT:
            if (event->user.code != SDL_EVENT_USER_TRIGGER)
                keys_held_down[kLEFT_ARROW].pressed = true;
            break;
        case SDLK_RIGHT:
            if (event->user.code != SDL_EVENT_USER_TRIGGER)
                keys_held_down[kRIGHT_ARROW].pressed = true;
            break;
        case SDLK_UP:
            if (event->user.code != SDL_EVENT_USER_TRIGGER)
                keys_held_down[kUP_ARROW].pressed = true;
            break;
        case SDLK_DOWN:
            if (event->user.code != SDL_EVENT_USER_TRIGGER)
                keys_held_down[kDOWN_ARROW].pressed = true;
            break;
            
        default: break;
    }


    //for Dany0 (azerty testing)
    //while holding n, will show key struct info
    //if (keystate['r'] != 0)
    //{
        //printf("scancode = %d\n", (int)event->key.keysym.scancode);
        //printf("keysym = %d\n", (int)event->key.keysym.sym);
        //printf("\n");
    //}
}

// key up
void key_up_handler(SDL_Event* event)
{

    if (input_state.skeleton_editor)
    {
        SkeletonEditor::key_up_handler(event);
        return;
    }

    if (input_state.agent_container || input_state.block_container)
        container_key_up_handler(event);
    else if (input_state.chat)
        chat_key_up_handler(event);
    else
    {
        if (input_state.input_mode == INPUT_STATE_AGENT)
            agent_key_up_handler(event);
        else
            camera_key_up_handler(event);

        switch (event->key.keysym.sym)
        {
            default: break;
        }
    }

    switch (event->key.keysym.sym)
    {
        //case SDLK_LALT:
        //case SDLK_RALT:
            //input_state.mouse_bound = true;
            //break;

        case SDLK_BACKSPACE:
            keys_held_down[kBACKSPACE].pressed = false;
            break;
        case SDLK_LEFT:
            keys_held_down[kLEFT_ARROW].pressed = false;
            break;
        case SDLK_RIGHT:
            keys_held_down[kRIGHT_ARROW].pressed = false;
            break;
        case SDLK_UP:
            keys_held_down[kUP_ARROW].pressed = false;
            break;
        case SDLK_DOWN:
            keys_held_down[kDOWN_ARROW].pressed = false;
            break;

        default: break;
    }

}

// mouse down
void mouse_button_down_handler(SDL_Event* event)
{
    if (input_state.last_mouse_button_down_event_frame == ClientState::frame_id) return;
    input_state.last_mouse_button_down_event_frame = ClientState::frame_id;
    if (input_state.skeleton_editor)
    {
        SkeletonEditor::mouse_button_down_handler(event);
        return;
    }

    // chat doesnt affect mouse
    if (input_state.agent_container || input_state.block_container)
        container_mouse_down_handler(event);
    else if (input_state.input_mode == INPUT_STATE_AGENT)
        agent_mouse_down_handler(event);
    else
        camera_mouse_down_handler(event);

    switch (event->button.button)
    {
        default: break;
    }
    
}

// mouse up
void mouse_button_up_handler(SDL_Event* event)
{
    if (input_state.last_mouse_button_up_event_frame == ClientState::frame_id) return;
    input_state.last_mouse_button_up_event_frame = ClientState::frame_id;
    if (input_state.skeleton_editor)
    {
        SkeletonEditor::mouse_button_up_handler(event);
        return;
    }

    // chat doesnt affect mouse

    if (input_state.agent_container || input_state.block_container)
        container_mouse_up_handler(event);
    else if (input_state.input_mode == INPUT_STATE_AGENT)
        agent_mouse_up_handler(event);
    else
        camera_mouse_up_handler(event);

    switch (event->button.button)
    {
        default: break;
    }
}

// mouse motion
void mouse_motion_handler(SDL_Event* event)
{
    // chat doesnt affect mouse

    if (input_state.ignore_mouse_motion)
    {
        input_state.ignore_mouse_motion = false;
        return;
    }

    if (input_state.agent_container || input_state.block_container)
    {
        SDL_ShowCursor(1);  // always show cursor (until we have our own cursor)
        container_mouse_motion_handler(event);
    }
    else if (input_state.input_mode == INPUT_STATE_AGENT)
        agent_mouse_motion_handler(event);
    else
        camera_mouse_motion_handler(event);

    switch (event->button.button)
    {
        default: break;
    }
}

// quit
void quit_event_handler(SDL_Event* event)
{
    input_state.quit = true;
}

// keyboard state
void key_state_handler(Uint8 *keystate, int numkeys)
{
    if (input_state.skeleton_editor)
    {
        SkeletonEditor::key_state_handler(keystate, numkeys);
        return;
    }
    
    char f,b,l,r,jet,jump,crouch,boost,m1,m2,m3;
    f=b=l=r=jet=jump=crouch=boost=m1=m2=m3=0;
    
    if (!input_state.chat)
    {
        if (input_state.input_mode == INPUT_STATE_AGENT)
            agent_key_state_handler(keystate, numkeys, &f, &b, &l, &r, &jet, &jump, &crouch, &boost, &m1, &m2, &m3);
        else
            camera_key_state_handler(keystate, numkeys);
    }

    // always set control state
    ClientState::playerAgent_state.set_control_state(f,b,l,r,jet,jump,crouch,boost,m1,m2,m3, agent_camera->theta, agent_camera->phi);
}

// active event (window / input focus)
void active_event_handler(SDL_Event* event)
{
    //Uint8 app_state = SDL_GetAppState();

    //if (app_state & SDL_APPACTIVE)
    //{
        ////if (event->active.gain)
            ////input_state.has_focus = true;
        ////else
            ////input_state.has_focus = false;
    //}
}
