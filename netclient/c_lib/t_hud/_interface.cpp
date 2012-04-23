#include "_interface.hpp"

#include <c_lib/t_hud/item_grid.hpp>
#include <c_lib/t_hud/inventory_hud.hpp>
#include <c_lib/t_hud/tool_belt_hud.hpp>

namespace t_hud
{

/*
	Init
*/
void draw_init()
{
    init_item_grid();
}

void draw_teardown()
{

}
/*
	Input Handling
*/


void enable_inventory_hud()
{

}

void disable_inventory_hud()
{

}



void handle_click(int x, int y, int button)
{

}

void set_mouse_position(int x, int y)
{

}

/*
	Drawing
*/
void draw_hud()
{
    if (!input_state.inventory) return;

    draw_inventory_hud();
    draw_toolbelt_hud();
    //static ItemGrid g;
    //g.draw(300,300);
}


}
