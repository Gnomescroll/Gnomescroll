#include "options.hpp"

//#include <stdlib.h>


namespace Options
{

/* User */
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

}
