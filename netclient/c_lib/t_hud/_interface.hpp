#pragma once

namespace t_hud
{

void draw_hud();

/*
Input Handling
*/

void handle_click(int x, int y, int button);

void set_mouse_position(int x, int y);
/*
Init
*/
void draw_init();
void draw_teardown();

}