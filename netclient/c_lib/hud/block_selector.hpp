#pragma once

#include <compat_gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <c_lib/hud/texture_loader.h>
#include <c_lib/SDL/draw_functions.h>

int _init_block_selector();
void _draw_block_selector(int x, int y);
void _load_block_selector_texture(char *file, int scale);
int _blit_block_selector(int texture, float x0, float y0, float x1, float y1, float z);

/* COPIED FROM cube_select.hpp */

void _init_cube_select();
int _get_selected_cube_id(); //get the cube selected by hud
void _cube_select_set_hud(int pos, int cube_id, int tex_id); //set the cubes in each position on hud
void _draw_cube_selector(float x, float y, float size, int mode);
void _hud_control_input(int pos);
