#pragma once

//#include <common/compat_gl.h>


const int MOUSE_INPUT_BUFFER_SIZE = 32;
const float MOUSE_BUFFER_DECAY = 0.70f;

struct MouseMotionAverage* get_mouse_render_state(int t);

int init_input();

int get_key_state();
int process_events();

void poll_mouse();

char getUnicodeValue(SDL_keysym keysym);

void bind_mouse();
void unbind_mouse();
