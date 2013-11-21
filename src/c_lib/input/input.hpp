/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

struct MouseMotionAverage* get_mouse_render_state(int t);

int init_input();
void teardown_input();

int get_key_state();
int process_events();

void poll_mouse();

char getUnicodeValue(SDL_keysym keysym);

void bind_mouse();
void unbind_mouse();
