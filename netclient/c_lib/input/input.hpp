#pragma once

#include <compat_gl.h>

struct MouseMotionAverage {
    float x,y;
};

#define MOUSE_INPUT_BUFFER_SIZE 16
#define MOUSE_BUFFER_DECAY 0.50f
struct MouseMotionAverage* get_mouse_render_state(int t);

int init_input();

int get_key_state();
int process_events();
void pan_camera(int delta_tick);

char getUnicodeValue(SDL_keysym keysym);
