#include "loop.hpp"

#include <c_lib/state/client_state.hpp>
#include <c_lib/SDL/SDL_functions.h>
#include <c_lib/sound/sound.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/camera/camera.hpp>
#include <c_lib/animations/animations.hpp>
#include <net_lib/host.hpp>
#include <c_lib/time/physics_timer.h>
#include <c_lib/t_map/t_vbo.hpp>
#include <c_lib/options.hpp>

int get_mouse_tick()
{
    static int last = 0;
    int current = _get_ticks();
    int delta = current - last;
    last = current;
    return delta;
}

void connect()
{   // parse IP address string

    int address[4];
    int address_index = 0;

    char c;
    int i=0, j=0;
    char tmp[3+1];
    while ((c = Options::server[i++]) != '\0')
    {
        if (c == '.')
        {
            tmp[j] = '\0';
            address[address_index++] = atoi(tmp);
            j = 0;
            continue;
        }
        tmp[j++] = c;
    }
    tmp[j] = '\0';
    address[address_index] = atoi(tmp);

    client_connect_to(address[0], address[1], address[2], address[3], Options::port);
}

void init_main_loop()
{
    _set_resolution(Options::width, Options::height, Options::fullscreen);
    init_c_lib();
    ClientState::set_desired_name(Options::name);
    _START_CLOCK(); // must start before networking
    connect();
}

int main_loop()
{
    
/* BEGIN SETUP */
    int ping_ticks = _get_ticks();
    
    int fps_average_index = 0;
    int fps_average[30+1];
    int fps_last_tick = _get_ticks();
    float fps_value = 0.0f;
/* END SETUP */

    // update mouse
    pan_camera(get_mouse_tick());

    while (!input_state.quit)
    {

        // update mouse
        pan_camera(get_mouse_tick());
        
        // physics loop
        int physics_ticks = 0;
        while (1)
        {
            int tick_count = _GET_TICK();
            if (tick_count == 0 || physics_ticks > 0)
                break;
            physics_ticks++;

            // input
            process_events();
            get_key_state();

            // tick animations
            Animations::animations_tick();
            
            // tick client state
            ClientState::tick_client_state();

            // update sound listener
            ClientState::playerAgent_state.update_sound();

            // update mouse
            pan_camera(get_mouse_tick());
        }

        if (physics_ticks >= 2)
            printf("Physics: %d ticks this frame\n", physics_ticks);

        // update mouse
        pan_camera(get_mouse_tick());
            
        // update hud projected names
        ClientState::playerAgent_state.display_agent_names();

        // dispatch
        client_dispatch_network_events();
        // flush network
        flush_to_net();

        // set input options (set these in an options struct at load)   TODO
        set_input_options(
            Options::invert_mouse,
            Options::sensitivity
        );

        // update mouse
        pan_camera(get_mouse_tick());

        // update camera state
        ClientState::update_camera();

        // switch to world projection
        world_projection();

        // draw map TODO
        t_map::draw_map();
    
        // draw client state
        ClientState::draw_client_state();

        // draw animations
        Animations::animations_draw();

        // update mouse
        pan_camera(get_mouse_tick());

        if (Options::hud)
        {
            // switch to hud  projection
            hud_projection();

            // draw hud
            Hud::set_hud_fps_display(fps_value);
            Hud::update_hud_draw_settings();
            Hud::draw_hud();
        }

        // flip sdl
        _swap_buffers();

        // do fps calculation
        if (Options::fps)
        {
            int fps_current_tick = _get_ticks();
            fps_average[fps_average_index++] = fps_current_tick - fps_last_tick;
            fps_last_tick = fps_current_tick;
            if (fps_average_index > 30)
            {
                int sum = 0;
                for (int i=0; i<fps_average_index; i++)
                    sum += fps_average[i];

                fps_value = ((float)sum) / ((float)fps_average_index);
                fps_average_index = 0;
            }
        }

        // check ping throttle
        if (Options::ping)
        {
            int ping_now = _get_ticks();
            if (ping_now - ping_ticks > Options::ping_update_interval)
            {
                ping_ticks = ping_now;
                ClientState::send_ping();
            }
        }

        // update mouse
        pan_camera(get_mouse_tick());

        // update sound
        Sound::update_sound();
        // update client_state
        ClientState::update_client_state();
    }

    close_c_lib();
    
    return 0;
}
