#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <options/option_macros.hpp>

namespace Options
{

/* Network */
OPT_STRING_HEADER(server)
OPT_INT_HEADER(port)

/* Window / Camera */
OPT_INT_HEADER(width)
OPT_INT_HEADER(height)
OPT_BOOL_HEADER(fullscreen)
OPT_FLOAT_HEADER(fov)

/* Controls */
OPT_FLOAT_HEADER(sensitivity)
OPT_FLOAT_HEADER(camera_speed)
OPT_BOOL_HEADER(invert_mouse)
OPT_BOOL_HEADER(mouse_bound)

/* HUD / Info */
OPT_INT_HEADER(ping_update_interval)
OPT_INT_HEADER(system_message_r)
OPT_INT_HEADER(system_message_g)
OPT_INT_HEADER(system_message_b)
OPT_BOOL_HEADER(player_chat);

/* Font */
OPT_STRING_HEADER(font)

/* Sound */
OPT_BOOL_HEADER(sound)
OPT_INT_HEADER(sfx)
OPT_INT_HEADER(music)
OPT_STRING_HEADER(sound_device)

/* Graphics */
OPT_INT_HEADER(animation_level)
OPT_INT_HEADER(view_distance)
OPT_BOOL_HEADER(placement_outline);
OPT_BOOL_HEADER(mesa_shader);

/* Logging */
OPT_BOOL_HEADER(logger)

/* Help */
OPT_BOOL_HEADER(show_tips);

void register_options();

void validate();

}   // Options
