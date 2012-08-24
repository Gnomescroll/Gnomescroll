#pragma once

#include <options/option_macros.hpp>

namespace Options
{

/* User */
OPT_STRING_HEADER(name) // player_name

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

/* HUD / Info */
OPT_INT_HEADER(ping_update_interval)
OPT_INT_HEADER(system_message_r)
OPT_INT_HEADER(system_message_g)
OPT_INT_HEADER(system_message_b)

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

}   // Options
