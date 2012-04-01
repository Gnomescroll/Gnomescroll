#pragma once

#include <c_lib/option_macros.hpp>

namespace Options
{

/* User */
OPT_STRING_HEADER(name)

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

///

OPT_STRING(name, "default-name")

/* Network */
OPT_STRING(server, "127.0.0.1")
OPT_INT(port, 4096)

/* Game preferences */
OPT_BOOL(auto_assign_team, true)

/* Window / Camera */
OPT_INT(width, 1024)
OPT_INT(height, 800)
OPT_BOOL(fullscreen, false)
OPT_FLOAT(fov, 85.0f)

/* Controls */
OPT_FLOAT(sensitivity, 70.0f)
OPT_FLOAT(camera_speed, 0.6)
OPT_BOOL(invert_mouse, false)

/* HUD / Info */
OPT_BOOL(hud, true)
OPT_BOOL(diagnostic_hud, true)
OPT_BOOL(fps, true)
OPT_BOOL(ping, true)
OPT_INT(ping_update_interval, 500)

/* Font */
OPT_STRING(font, "inconsolata_18.fnt")

/* Sound */
OPT_BOOL(sound, false)
OPT_INT(sfx, 100)
OPT_INT(music, 100)
OPT_STRING(sound_device, "")

/* Graphics */
OPT_BOOL(animations, true)

/* Logging */
OPT_BOOL(logger, false)


void register_options()
{


}


void register_options();

}
