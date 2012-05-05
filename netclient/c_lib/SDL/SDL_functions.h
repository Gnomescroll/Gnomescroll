#pragma once

int _set_resolution(int xres, int yres, int fullscreen);

int VersionMismatchBox();

int init_video();
void _del_video();
int _swap_buffers();

//int _get_ticks();  // use _GET_MS_TIME()

void close_SDL();

void save_screenshot();

extern int _xres;
extern int _yres;
extern float _xresf;
extern float _yresf;
extern int _fullscreen;
