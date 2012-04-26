#pragma once

#include <c_lib/common/option_macros.hpp>

namespace Options
{

/* User */
OPT_STRING_HEADER(name) // player_name

/* Network */
OPT_STRING_HEADER(server)
OPT_INT_HEADER(port)

/* Game preferences */
OPT_BOOL_HEADER(auto_assign_team)

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
OPT_BOOL_HEADER(hud)
OPT_BOOL_HEADER(diagnostic_hud)
OPT_BOOL_HEADER(fps)
OPT_BOOL_HEADER(ping)
OPT_INT_HEADER(ping_update_interval)

/* Font */
OPT_STRING_HEADER(font)

/* Sound */
OPT_BOOL_HEADER(sound)
OPT_INT_HEADER(sfx)
OPT_INT_HEADER(music)
OPT_STRING_HEADER(sound_device)

/* Graphics */
OPT_BOOL_HEADER(animations)

/* Logging */
OPT_BOOL_HEADER(logger)

void register_options();

}
