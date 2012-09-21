#include "handlers.hpp"

#include <math.h>
#include <chat/client.hpp>
#include <input/skeleton_editor.hpp>
#include <input/equipped_sprite_adjuster.hpp>

//#include <hud/hud.hpp>
#include <hud/_interface.hpp>

//toggling graphics settings
#include <t_map/glsl/shader.hpp>

#include <t_hud/_interface.hpp>
#include <hud/hud.hpp>

InputState input_state;

bool mouse_unlocked_for_ui_element()
{   // if mouse was unlocked to allow control of a ui element
    return (input_state.container_block || input_state.agent_container || input_state.awesomium);
}

// triggers
void toggle_mouse_bind()
{
    input_state.mouse_bound = (!input_state.mouse_bound);
}

void toggle_help_menu()
{
    input_state.help_menu = (!input_state.help_menu);
    if (input_state.help_menu)
        Hud::clear_prompt(Hud::press_help_text);
}

void enable_agent_container()
{
    if (input_state.agent_container) return;

    input_state.agent_container = true;
    
    // force set the mouse position
    // because we wont have a motion event coming in initially sometimes
    SDL_ShowCursor(0);
    int x,y;
    SDL_GetMouseState(&x, &y);
    t_hud::set_mouse_position(x,y);

    t_hud::enable_agent_container_hud();
    ItemContainer::open_inventory();
    input_state.rebind_mouse = input_state.mouse_bound;
    input_state.mouse_bound = false;
}

void disable_agent_container()
{
    if (!input_state.agent_container) return;
    input_state.agent_container = false;
    // poll mouse button state
    // if left or right is unpressed, trigger an up event
    int x,y;
    Uint8 btns = SDL_GetMouseState(&x,&y);
    if (!(btns & SDL_BUTTON_LEFT)) Toolbelt::left_trigger_up_event();
    if (!(btns & SDL_BUTTON_RIGHT)) Toolbelt::right_trigger_up_event();
    
    t_hud::disable_agent_container_hud();
    ItemContainer::close_inventory();
    //if (input_state.input_focus)    // dont change mouse state if we're not in focus. it grabs the window
    if (SDL_GetAppState() & SDL_APPINPUTFOCUS)    // dont change mouse state if we're not in focus. it grabs the window
        input_state.mouse_bound = input_state.rebind_mouse;
    input_state.ignore_mouse_motion = true;
}

void toggle_agent_container()
{
    if (input_state.agent_container) disable_agent_container();
    else enable_agent_container();
}

void enable_container_block(int container_id)
{
    if (input_state.container_block) return;
    
    GS_ASSERT(container_id != NULL_CONTAINER);
    
    // release all toolbelt
    Toolbelt::left_trigger_up_event();
    Toolbelt::right_trigger_up_event();
    
    // force set the mouse position
    // because we wont have a motion event coming in initially sometimes
    SDL_ShowCursor(0);
    int x,y;
    SDL_GetMouseState(&x, &y);
    t_hud::set_mouse_position(x,y);
    
    input_state.container_block = true;
    input_state.container_block_id = container_id;
    t_hud::enable_container_block_hud(container_id);
    input_state.rebind_mouse = input_state.mouse_bound;
    input_state.mouse_bound = false;
}

void disable_container_block()
{
    if (!input_state.container_block) return;
    input_state.container_block = false;
    t_hud::disable_container_block_hud();
    if (input_state.input_focus)    // dont change mouse state if we're not in focus. it grabs the window
        input_state.mouse_bound = input_state.rebind_mouse;
    input_state.ignore_mouse_motion = true;
}

void close_all_containers()
{
    disable_container_block();
    disable_agent_container();
    // close any public containers we had open
    if (ItemContainer::opened_container == NULL_CONTAINER) return;
    int container_id = ItemContainer::opened_container;
    ItemContainer::close_container(container_id);
    ItemContainer::send_container_close(container_id);
}

void toggle_scoreboard()
{
    input_state.scoreboard = (!input_state.scoreboard);
}

void toggle_map()
{
    input_state.map = (!input_state.map);
}

void disable_chat()
{
    if (!input_state.chat) return;
    input_state.chat = false;
}

void enable_chat()
{
    if (input_state.chat) return;
    input_state.chat = true;
}

void toggle_chat()
{
    if (input_state.chat)
        disable_chat();
    else
        enable_chat();
}

void toggle_full_chat()
{
    input_state.full_chat = (!input_state.full_chat);
}

void toggle_hud()
{
    input_state.draw_hud = (!input_state.draw_hud);
}

void toggle_admin_controls()
{
    input_state.admin_controls = (!input_state.admin_controls);
}

void disable_awesomium()
{
    #if GS_AWESOMIUM
    if (!input_state.awesomium) return;
    printf("disable awesomium\n");
    input_state.awesomium = false;
    input_state.mouse_bound = input_state.rebind_mouse;
    input_state.ignore_mouse_motion = true;
    Awesomium::disable();
    #endif
}

void enable_awesomium()
{
    #if GS_AWESOMIUM
    if (input_state.awesomium) return;
    printf("enable awesomium\n");
    input_state.awesomium = true;
    input_state.rebind_mouse = input_state.mouse_bound;
    input_state.mouse_bound = false;
    Awesomium::enable();
    Hud::clear_prompt(Hud::open_login_text);
    #endif
}

void toggle_awesomium()
{
    #if GS_AWESOMIUM
    if (input_state.awesomium)
        disable_awesomium();
    else
        enable_awesomium();
    #endif
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
    input_state.confirm_quit = false;
    enable_quit();  // quit automatically in debug
    #endif
}

void toggle_skeleton_editor()
{
    input_state.skeleton_editor = (!input_state.skeleton_editor);
    if (input_state.skeleton_editor)
        printf("Skeleton editor enabled\n");
    else
        printf("Skeleton editor disabled\n");
}

void toggle_equipped_sprite_adjuster()
{
    input_state.equipped_sprite_adjuster = (!input_state.equipped_sprite_adjuster);
    if (input_state.equipped_sprite_adjuster)
        printf("Equipped sprite adjuster enabled\n");
    else
        printf("Equipped sprite adjuster disable\n");
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

void init_input_state()
{   // set input_state defaults
    #if PRODUCTION
    input_state.mouse_bound = true;
    input_state.input_mode = INPUT_STATE_AGENT;
    input_state.camera_mode = INPUT_STATE_AGENT;
    #else
    input_state.mouse_bound = false;
    input_state.input_mode = INPUT_STATE_CAMERA;
    input_state.camera_mode = INPUT_STATE_CAMERA;
    #endif

    input_state.login_mode = false;

    input_state.rebind_mouse = input_state.mouse_bound;

    input_state.draw_hud = true;
    input_state.vbo_debug = false;
    input_state.diagnostics = false;
    input_state.admin_controls = false;

    input_state.help_menu = false;
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

    // containers
    input_state.agent_container = false;
    input_state.container_block = false;
    input_state.container_block_id = NULL_CONTAINER;

    // options
    input_state.invert_mouse = false;
    input_state.sensitivity = 100.0f;

    // debug
    input_state.frustum = true;

    // awesomium
    input_state.awesomium = false;

    input_state.error_message = Hud::has_error();

    // SDL state
    // these starting conditions are variable, so dont rely on them for deterministic logic
    Uint8 app_state = SDL_GetAppState();
    input_state.input_focus = (app_state & SDL_APPMOUSEFOCUS);
    input_state.mouse_focus = (app_state & SDL_APPINPUTFOCUS);
    input_state.app_active   = (app_state & SDL_APPACTIVE);
}

// options
const float ZOOM_SENSITIVITY_SCALE = 0.7f;
void update_input_state()
{
    input_state.invert_mouse = Options::invert_mouse;
    input_state.sensitivity = Options::sensitivity;
    if (Hud::hud_draw_settings.zoom)
        input_state.sensitivity *= ZOOM_SENSITIVITY_SCALE;

    Uint8 app_state = SDL_GetAppState();
    input_state.input_focus = (app_state & SDL_APPMOUSEFOCUS);
    input_state.mouse_focus = (app_state & SDL_APPINPUTFOCUS);
    input_state.app_active   = (app_state & SDL_APPACTIVE);

    bool had_error = input_state.error_message;
    input_state.error_message = Hud::has_error();
    if (!had_error && input_state.error_message)
    {
        disable_agent_container();
        disable_container_block();
        disable_chat();
    }
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
    void chat_key_down_handler(SDL_Event* event);   // forward decl
    if (!input_state.chat) return;

    update_keys_held_down();
    static SDL_Event event;
    for (int i=0; i<KEYS_HELD_DOWN; i++)
    {
        if (keys_held_down[i].t > KEY_HOLD_DELAY
          && (keys_held_down[i].t - KEY_HOLD_DELAY) % KEY_HOLD_RATE == 0)
        {
            event.user.code = SDL_EVENT_USER_TRIGGER;
            event.key.keysym.sym = keys_held_down_map[i];
            chat_key_down_handler(&event);
        }
    }
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
    //if (ClientState::playerAgent_state.you == NULL) return;
    //if (ClientState::playerAgent_state.you->status.dead) return;
    switch (event->key.keysym.sym)
    {
        case SDLK_e:
        case SDLK_ESCAPE:
            close_all_containers();
            break;

        case SDLK_t:
        case SDLK_RETURN:
            toggle_chat();
            chat_client->use_global_channel();
            break;

        default: break;
    }
}

void container_key_up_handler(SDL_Event* event)
{
    if (ClientState::playerAgent_state.you == NULL) return;
    if (ClientState::playerAgent_state.you->status.dead) return;
}

void container_mouse_down_handler(SDL_Event* event)
{
    if (ClientState::playerAgent_state.you == NULL) return;
    if (ClientState::playerAgent_state.you->status.dead) return;

    // gets correct mouse pixels
    int x,y;
    SDL_GetMouseState(&x, &y);

    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            t_hud::left_mouse_down(x,y);
            break;

        case SDL_BUTTON_RIGHT:
            t_hud::right_mouse_down(x,y);
            break;

        default:
            return;
    }
}

void container_mouse_up_handler(SDL_Event* event)
{
    if (ClientState::playerAgent_state.you == NULL) return;
    if (ClientState::playerAgent_state.you->status.dead) return;
    
    Toolbelt::left_trigger_up_event(); // clear any trigger events
    Toolbelt::right_trigger_up_event(); // clear any trigger events
    
    if (!input_state.mouse_bound // for whatever reason it only needs to be done when mouse is unbound
      && input_state.ignore_next_right_click_event
      && event->button.button == SDL_BUTTON_RIGHT)
    {
        input_state.ignore_next_right_click_event = false;
        return;
    }
    
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
            //printf("container event: ");
            //printf("id %d ", container_event.container_id);
            //printf("slot %d ", container_event.slot);
            //printf("synthesizer %d ", container_event.synthesizer);
            //printf("craft output %d ", container_event.craft_output);
            //printf("\n");
            ItemContainer::mouse_left_click_handler(container_event.container_id, container_event.slot, container_event.alt_action);
            break;

        case SDL_BUTTON_RIGHT:
            container_event = t_hud::right_mouse_up(x,y);
            ItemContainer::mouse_right_click_handler(container_event.container_id, container_event.slot, container_event.alt_action);
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

void agent_key_down_handler(SDL_Event* event)
{
    t_hud::ContainerInputEvent container_event;
    switch (event->key.keysym.sym)
    {
        //case SDLK_9:
            //t_mech::add_mech();
            //break;

        //case SDLK_0:
            //t_mech::rotate_mech();
            //break;

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
    if (keystate[SDLK_w])
        *f = 1;
    if (keystate[SDLK_s])
        *b = 1;
    if (keystate[SDLK_a])
        *l = 1;
    if (keystate[SDLK_d])
        *r = 1;
    if (keystate[SDLK_LSHIFT])  // LSHIFT
        *jet = 1;
    if (keystate[SDLK_SPACE] || keystate[SDLK_PAGEDOWN])  // 2nd "bind" for CorpusC's keyboard
        *jump = 1;
    if (keystate[SDLK_LCTRL])  // LCTRL
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

void print_mob_id()
{
    // hitscan against mobs
    GS_ASSERT(current_camera != NULL);
    if (current_camera == NULL) return;
    
    struct Vec3 p = current_camera->get_position();
    struct Vec3 v = current_camera->forward_vector();
    int ignore_id = -1;
    ObjectType ignore_type = OBJECT_NONE;

    class Voxel_hitscan_target target;
    float vox_distance = 0.0f;
    float collision_point[3] = {0.0f};
    bool voxel_hit = STATE::voxel_hitscan_list->hitscan(
        p.x, p.y, p.z,
        v.x, v.y, v.z,
        ignore_id, ignore_type,
        collision_point, &vox_distance,
        &target
    );
    if (!voxel_hit) return;

    printf("mob id: %d\n", target.entity_id);
}

void camera_key_up_handler(SDL_Event* event){}
void camera_mouse_down_handler(SDL_Event* event)
{
    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            print_mob_id();
            break;
            
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
    
    if (keystate[SDLK_w])
        free_camera->move(speed, 0,0);
    if (keystate[SDLK_s])
        free_camera->move(-speed,0,0);
    if (keystate[SDLK_a])
        free_camera->move(0, speed, 0);
    if (keystate[SDLK_d])
        free_camera->move(0,-speed,0);
    if (keystate[SDLK_r])
        free_camera->move(0,0,speed);
    if (keystate[SDLK_f])
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

    if (input_state.equipped_sprite_adjuster)
    {
        switch (event->key.keysym.sym)
        {
            case SDLK_ESCAPE:
            case SDLK_l:
                toggle_equipped_sprite_adjuster();
                return;
            default:
                break;
        }

        EquippedSpriteAdjuster::key_down_handler(event);
        return;
    }

    if (input_state.awesomium)
    {
        switch (event->key.keysym.sym)
        {
            case SDLK_ESCAPE:
                if (input_state.login_mode)
                    enable_quit();  // quit automatically if we are in login mode
                else
                    toggle_awesomium();
                break;

            default:
                Awesomium::SDL_keyboard_event(event);
                break;
        }
    }
    else if (input_state.chat)
        chat_key_down_handler(event);
    else if (input_state.agent_container || input_state.container_block)
        container_key_down_handler(event);
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
                if (input_state.admin_controls)
                    Animations::agent_bleed(
                        ClientState::playerAgent_state.camera_state.x,
                        ClientState::playerAgent_state.camera_state.y,
                        ClientState::playerAgent_state.camera_state.z
                   );
                break;
                
            case SDLK_g:
                if (input_state.admin_controls)
                    toggle_camera_mode();
                break;
                
            case SDLK_h:
                toggle_help_menu();
                break;

            case SDLK_k:
                if (input_state.admin_controls)
                    input_state.frustum = (!input_state.frustum);
                break;

            case SDLK_l:
                #if !PRODUCTION
                if (input_state.admin_controls)
                    toggle_equipped_sprite_adjuster();
                #endif
                break;

            case SDLK_SEMICOLON:
                if (input_state.admin_controls)
                {
                    Options::animation_level %= 3;
                    Options::animation_level += 1;
                    printf("Animation level: %d\n", Options::animation_level);
                }
                break;

            case SDLK_m:
                if (input_state.admin_controls)
                    toggle_map();
                break;

            case SDLK_n:
                if (input_state.confirm_quit)
                    toggle_confirm_quit();
                break;

            case SDLK_o:
                #if !PRODUCTION
                if (input_state.admin_controls)
                    toggle_skeleton_editor();
                #endif
                break;
                
            
            case SDLK_t:
                if (input_state.admin_controls)
                    toggle_input_mode();
                else
                {
                    toggle_chat();
                    chat_client->use_global_channel();
                }
                break;
                
            case SDLK_u:
                #if PRODUCTION
                if (input_state.admin_controls)
                #endif
                    toggle_mouse_bind();
                break;

            case SDLK_y:
                if (input_state.confirm_quit)
                    enable_quit();
                break;

            case SDLK_TAB:
                toggle_scoreboard();
                break;

            case SDLK_RETURN:
                toggle_chat();
                chat_client->use_global_channel();
                break;

            case SDLK_SLASH:
                if (input_state.admin_controls)
                    toggle_hud();
                break;

            case SDLK_LEFTBRACKET:
                if (input_state.admin_controls)
                    ClientState::playerAgent_state.toggle_camera_mode();
                break;

            case SDLK_QUOTE:
                toggle_graphs();
                break;

            case SDLK_e:
                toggle_agent_container();
                break;

            case SDLK_ESCAPE:
                if (Hud::has_error())
                    enable_quit();
                else
                    toggle_confirm_quit();
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
            if (input_state.admin_controls) t_map::toggle_3d_texture_settings();
            break;

        case SDLK_F1:
            if (!input_state.login_mode)
                toggle_awesomium();
            break;

        case SDLK_F2:
            input_state.diagnostics = (!input_state.diagnostics);
            break;

        case SDLK_F3:
            input_state.vbo_debug = (!input_state.vbo_debug);
            break;

        case SDLK_F12:
            toggle_admin_controls();
            break;

        case SDLK_LALT:
        case SDLK_RALT:
            if (input_state.admin_controls)
            {
                input_state.mouse_bound = false;
                printf("Unbound mouse with alt\n");
            }
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
}

// key up
void key_up_handler(SDL_Event* event)
{
    if (input_state.skeleton_editor)
    {
        SkeletonEditor::key_up_handler(event);
        return;
    }

    if (input_state.equipped_sprite_adjuster)
    {
        EquippedSpriteAdjuster::key_up_handler(event);
        return;
    }

    if (input_state.awesomium)
        Awesomium::SDL_keyboard_event(event);
    else if (input_state.chat)
        chat_key_up_handler(event);
    else if (input_state.agent_container || input_state.container_block)
        container_key_up_handler(event);
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
    if (input_state.awesomium)
        Awesomium::SDL_mouse_event(event);
    else if (input_state.agent_container || input_state.container_block)
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

    if (input_state.awesomium)
        Awesomium::SDL_mouse_event(event);
    else if (input_state.agent_container || input_state.container_block)
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

    if (input_state.awesomium)
    {
        SDL_ShowCursor(1);  // always show cursor (until we have our own cursor)
        Awesomium::SDL_mouse_event(event);
    }
    else if (input_state.agent_container || input_state.container_block)
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
    
    if (!input_state.chat && !input_state.awesomium)
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
    //SDL_APPMOUSEFOCUS -- mouse moves past visible portion of window
    //SDL_APPINPUTFOCUS -- window is selected. use this for detecting alt-tab
    //SDL_APPACTIVE     -- worthless, cannot trigger it

    bool gained = event->active.gain;

    // set input_state struct
    if (event->active.state & SDL_APPMOUSEFOCUS)
        input_state.mouse_focus = gained;
    else
    if (event->active.state & SDL_APPINPUTFOCUS)
        input_state.input_focus = gained;
    else
    if (event->active.state & SDL_APPACTIVE)
        input_state.app_active = gained;

    if (!mouse_unlocked_for_ui_element())
    {   // only do this if container/inventory not open 
        // handle alt tab
        if (event->active.state & SDL_APPINPUTFOCUS)
        {
            if (event->active.gain)
            {
                input_state.mouse_bound = input_state.rebind_mouse;
                input_state.rebind_mouse = false;
            }
            else
            {
                input_state.rebind_mouse = input_state.mouse_bound;
                input_state.mouse_bound = false;
            }
        }
    }

    //if (event->active.state & SDL_APPINPUTFOCUS || event->active.state & SDL_APPMOUSEFOCUS)
        //if (event->active.gain)
            //input_state.mouse_bound = input_state.rebind_mouse;
}
