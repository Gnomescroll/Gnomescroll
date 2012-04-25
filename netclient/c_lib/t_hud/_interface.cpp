#include "_interface.hpp"

#include <c_lib/t_hud/inventory_hud.hpp>
#include <c_lib/t_hud/tool_belt_hud.hpp>

namespace t_hud
{

int inventory_id = -1;
int toolbelt_id = -1;

int selected_slot_inventory = -1;
int selected_slot_x = 0;
int selected_slot_y = 0;

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


void enable_inventory_hud()
{
	printf("t_item::enable_inventory_hud \n");
}

void disable_inventory_hud()
{
	selected_slot_inventory = -1;
	printf("t_item::disable_inventory_hud \n");
}


void mouse_motion(int x, int y)
{
	//printf("t_item::mouse_motion x,y= %i %i \n", x,y);
}

void left_mouse_down(int x, int y)
{
	//printf("t_item::left_mouse_down \n");
	int ox,oy;

	//inventory click event
	if( inventory_hud_mouse_to_slot( x,y, &ox,&oy) )
	{
		if(inventory_id == -1)
		{
			printf("ERROR: inventory_id is -1 in t_hud \n");
			return;
		}

		selected_slot_inventory = inventory_id;
		selected_slot_x = ox;
		selected_slot_y = oy;
	}

	//toolbar click event
	if( toolbelt_hud_mouse_to_slot( x,y, &ox,&oy) )
	{
		if(toolbelt_id == -1)
		{
			printf("ERROR: inventory_id is -1 in t_hud \n");
			return;
		}

		selected_slot_inventory = toolbelt_id;
		selected_slot_x = ox;
		selected_slot_y = oy;
	}
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
    if (!input_state.inventory) return;

    draw_inventory_hud();
    draw_toolbelt_hud();
    //static ItemGrid g;
    //g.draw(300,300);
}


}
