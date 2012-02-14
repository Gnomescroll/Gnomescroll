#pragma once

namespace Options
{

#define OPT_BOOL_HEADER(NAME)\
extern bool NAME;\
void set_##NAME(bool val);

#define OPT_FLOAT_HEADER(NAME)\
extern float NAME;\
void set_##NAME(float val);

#define OPT_INT_HEADER(NAME)\
extern int NAME;\
void set_##NAME(int val);

#define OPT_STRING_HEADER(NAME)\
extern char* NAME;\
void set_##NAME(char* val);

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
OPT_INT_HEADER(font_size)

/* Sound */
OPT_BOOL_HEADER(sound)
OPT_INT_HEADER(sfx)
OPT_INT_HEADER(music)

}
