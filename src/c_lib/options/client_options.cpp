#include "client_options.hpp"

#include <common/lua/lua.hpp>

namespace Options
{

/* User */
OPT_STRING(name, "default-name")

/* Network */
OPT_STRING(server, "127.0.0.1")
OPT_INT(port, 4096)

/* Window / Camera */
OPT_INT(width, 1280)
OPT_INT(height, 800)
OPT_BOOL(fullscreen, false)
OPT_FLOAT(fov, 85.0f)

/* Controls */
OPT_FLOAT(sensitivity, 70.0f)
OPT_FLOAT(camera_speed, 0.6f)
OPT_BOOL(invert_mouse, false)

/* HUD / Info */
OPT_BOOL(hud, true)
OPT_BOOL(diagnostic_hud, true)
OPT_BOOL(fps, true)
OPT_BOOL(ping, true)
OPT_INT(ping_update_interval, 500)

/* Font */
OPT_STRING(font, "inc_18_b.fnt")

/* Sound */
OPT_BOOL(sound, true)
OPT_INT(sfx, 50)
OPT_INT(music, 50)
OPT_STRING(sound_device, "")

/* Graphics */
OPT_BOOL(animations, true)

/* Logging */
OPT_BOOL(logger, false)

/* Help */
OPT_BOOL(show_tips, true);

void register_options()
{
    /* User */
    OPT_STRING_REGISTER(name)

    /* Network */
    OPT_STRING_REGISTER(server)
    OPT_INT_REGISTER(port)

    /* Window / Camera */
    OPT_INT_REGISTER(width)
    OPT_INT_REGISTER(height)
    OPT_BOOL_REGISTER(fullscreen)
    OPT_FLOAT_REGISTER(fov)

    /* Controls */
    OPT_FLOAT_REGISTER(sensitivity)
    OPT_FLOAT_REGISTER(camera_speed)
    OPT_BOOL_REGISTER(invert_mouse)

    /* HUD / Info */
    OPT_BOOL_REGISTER(hud)
    OPT_BOOL_REGISTER(diagnostic_hud)
    OPT_BOOL_REGISTER(fps)
    OPT_BOOL_REGISTER(ping)
    OPT_INT_REGISTER(ping_update_interval)

    /* Font */
    OPT_STRING_REGISTER(font)

    /* Sound */
    OPT_BOOL_REGISTER(sound)
    OPT_INT_REGISTER(sfx)
    OPT_INT_REGISTER(music)
    OPT_STRING_REGISTER(sound_device)

    /* Graphics */
    OPT_BOOL_REGISTER(animations)

    /* Logging */
    OPT_BOOL_REGISTER(logger)

    /* Help */
    OPT_BOOL_REGISTER(show_tips);
}

}
