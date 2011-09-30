#pragma once

#include <compat_gl.h>

#include <stdio.h>
#include <stdlib.h>

#include <texture_loader.h>


int get_selected_cube_id(); //get the cube selected by hud

void cube_select_set_hud(int pos, int cube_id, int tex_id); //set the cubes in each position on hud

void draw_cube_selector(float x, float y);

void hud_control_input(int pos);
