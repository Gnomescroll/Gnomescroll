#pragma once

int _set_resolution(int xres, int yres, int fullscreen);

int init_video();
int _del_video();
int _swap_buffers();
int _get_ticks();
