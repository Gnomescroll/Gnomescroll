#pragma once

namespace t_hud
{

void draw_hud();

/*
Input Handling
*/

void enable_inventory_hud();
void disable_inventory_hud();

void handle_click(int x, int y, int button, bool down);
void set_mouse_position(int x, int y);
/*
Init
*/
void draw_init();
void draw_teardown();

}