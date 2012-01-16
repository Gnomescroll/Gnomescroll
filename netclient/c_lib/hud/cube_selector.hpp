#pragma once


void init_cube_selector();
int get_active_cube_id(); //get the cube selected by hud
int get_active_cube_pos(); //get the cube selected by hud
void set_cube_selector_property(int pos, int cube_id, int tex_id); //set the cubes in each position on hud
void draw_cube_selector(float x, float y, float size, int mode);
void set_active_cube_id(int id);    // set based on cube id
void set_active_cube_pos(int pos);  // set based on cube selector position

struct cube_select_element {
    int cube_id;
    int tex_id;
};
