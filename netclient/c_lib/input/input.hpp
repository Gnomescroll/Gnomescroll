#pragma once

#include <compat_gl.h>

struct MouseMotionAverage {
    float x,y;
};

#define MOUSE_INPUT_BUFFER_SIZE 16
#define MOUSE_BUFFER_DECAY 0.50f
struct MouseMotionAverage* get_mouse_render_state(int t);

//prototypes
int init_input(void);

typedef int (*key_event_func)(char key);
typedef int (*key_text_event_func)(char key, char* key_name, int event_state);

int _get_key_state();

int _process_events(key_event_func keyboard_event_cb, key_text_event_func keyboard_text_event_cb);

//call backs
int _key_event_callback(key_event_func user_func, char key);
int _key_text_event_callback(key_text_event_func user_func, char key, char* key_name, int event_state);

int _toggle_mouse_bind();

