#pragma once

int _set_resolution(int xres, int yres, int fullscreen);

int init_video();
void _del_video();
int _swap_buffers();
int _get_ticks();

extern int _xres;
extern int _yres;
extern float _xresf;
extern float _yresf;
extern int _fullscreen;
