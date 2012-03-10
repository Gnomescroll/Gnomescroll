#pragma once

//#include <compat_gl.h>


#define MOUSE_INPUT_BUFFER_SIZE 32
#define MOUSE_BUFFER_DECAY 0.70f

struct MouseMotionAverage* get_mouse_render_state(int t);

int init_input();

int get_key_state();
int process_events();

void poll_mouse();

char getUnicodeValue(SDL_keysym keysym);

void bind_mouse();
void unbind_mouse();
