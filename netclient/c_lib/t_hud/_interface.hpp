#pragma once

namespace t_hud
{

void draw_hud();

/*
Input Handling
*/

void enable_inventory_hud();
void disable_inventory_hud();

//void handle_left_mouse_click(int x, int y);
//void handle_right_mouse_click(int x, int y);


//void set_mouse_position(int x, int y);  // why?
//for dragging

void mouse_motion(int x, int y);

void left_mouse_down(int x, int y);
void left_mouse_up(int x, int y);
void right_mouse_down(int x, int y);
void right_mouse_up(int x, int y);


/*
Init
*/
void draw_init();
void draw_teardown();

}
