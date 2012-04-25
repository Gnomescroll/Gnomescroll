#include "_interface.hpp"

#include <c_lib/t_hud/inventory_hud.hpp>
#include <c_lib/t_hud/tool_belt_hud.hpp>

namespace t_hud
{

/*
    Init
*/
void draw_init()
{
    init_texture();
}

void draw_teardown()
{
    teardown_texture();
}
/*
    Input Handling
*/

static bool hud_enabled = false;
void enable_inventory_hud()
{
    hud_enabled = true;
}

void disable_inventory_hud()
{
    hud_enabled = false;
}


void mouse_motion(int x, int y)
{
    //printf("t_item::mouse_motion x,y= %i %i \n", x,y);
}

void left_mouse_down(int x, int y)
{
    //printf("t_item::left_mouse_down \n");
    int ox,oy;
    inventory_hud_mouse_to_slot( x,y, &ox,&oy);
	toolbelt_hud_mouse_to_slot( x,y, &ox,&oy);
}

void left_mouse_up(int x, int y)
{
    //printf("t_item::left_mouse_up \n");

}

void right_mouse_down(int x, int y)
{
    //printf("t_item::right_mouse_down \n");
}

void right_mouse_up(int x, int y)
{
    //printf("t_item::right_mouse_up \n");

}

/*
    Drawing
*/
void draw_hud()
{
    if (!hud_enabled) return;

    draw_inventory_hud();
    draw_toolbelt_hud();
    //static ItemGrid g;
    //g.draw(300,300);
}


}
