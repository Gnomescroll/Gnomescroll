#include "client_options.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <stdlib.h>

#include <options/argparse.hpp>
#include <common/lua/lua.hpp>

namespace Options
{

class SettingsExport* settings;

/* Network */
OPT_STRING(server)
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
OPT_BOOL(mouse_bound, true)

/* HUD / Info */
OPT_INT(ping_update_interval, 500)
OPT_INT(system_message_r, 255)
OPT_INT(system_message_g, 255)
OPT_INT(system_message_b, 50)
OPT_BOOL(player_chat, true);

/* Font */
OPT_STRING(font)

/* Sound */
OPT_BOOL(sound, true)
OPT_INT(sfx, 100)
OPT_INT(music, 100)
OPT_STRING(sound_device)

/* Graphics */
OPT_INT(animation_level, 3)
OPT_INT(view_distance, 128)
OPT_BOOL(placement_outline, false);
OPT_BOOL(mesa_shader, false);

/* Logging */
OPT_BOOL(logger, false)

/* Help */
OPT_BOOL(show_tips, true);

char* _test_string;

void register_options()
{
    GS_ASSERT(settings == NULL);
    settings = new SettingsExport;
    settings->track("test_string", &_test_string);

    /* Network */
    OPT_STRING_REGISTER(server, "127.0.0.1")
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
    OPT_BOOL_REGISTER(mouse_bound)

    /* HUD / Info */
    OPT_INT_REGISTER(ping_update_interval)
    OPT_INT_REGISTER(system_message_r)
    OPT_INT_REGISTER(system_message_g)
    OPT_INT_REGISTER(system_message_b)
    OPT_BOOL_REGISTER(player_chat);

    /* Font */
    OPT_STRING_REGISTER(font, "rdn.fnt")

    /* Sound */
    OPT_BOOL_REGISTER(sound)
    OPT_INT_REGISTER(sfx)
    OPT_INT_REGISTER(music)
    OPT_STRING_REGISTER(sound_device, "")

    /* Graphics */
    OPT_INT_REGISTER(animation_level)
    OPT_INT_REGISTER(view_distance)
    OPT_BOOL_REGISTER(placement_outline)
    OPT_BOOL_REGISTER(mesa_shader)

    /* Logging */
    OPT_BOOL_REGISTER(logger)

    /* Help */
    OPT_BOOL_REGISTER(show_tips);
}

void validate()
{
    OPT_INT_RANGE_VALID(animation_level, 0, 3);

    #if PRODUCTION
    ping_update_interval = 250;
    #else
    ping_update_interval = GS_MAX(1, ping_update_interval);
    #endif
}

}   // Options
