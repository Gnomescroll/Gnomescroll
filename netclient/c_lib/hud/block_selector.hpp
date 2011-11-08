#pragma once

#include <compat_gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <c_lib/hud/texture_loader.h>
#include <c_lib/SDL/draw_functions.h>

void init_cube_selector();
int _get_selected_cube_id(); //get the cube selected by hud
void _cube_select_set_hud(int pos, int cube_id, int tex_id); //set the cubes in each position on hud
void _draw_cube_selector(float x, float y, float size, int mode);
void _hud_control_input(int pos);

struct cube_select_element {
    int cube_id;
    int tex_id;
};
