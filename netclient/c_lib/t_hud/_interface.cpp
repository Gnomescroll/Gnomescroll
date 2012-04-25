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

static const int NULL_SLOT = -1;
static int active_slot = NULL_SLOT;


int get_inventory_and_slot(int x, int y, int* inventory_id, int* slot)
{
    // set default values ("none")
    //*inventory_id = -1;
    //*slot = NULL_SLOT;
    
    //// iterate inventories
    //float closest = 100000.0f;
    //int closest_id;
    //int closest_slot;

    //if (inventory_hud_mouse_to_slot(x,y, &ox,&oy))
    //{
        //closest = inventory_hud_z;
        //// fuck this, need object
    //}

    return 0;
}


void mouse_motion(int x, int y)
{
    //printf("t_item::mouse_motion x,y= %i %i \n", x,y);

    // if slot selected
    // draw that slot's icon at cursor
}

void left_mouse_down(int x, int y)
{
    //printf("t_item::left_mouse_down \n");
    int ox,oy;
    inventory_hud_mouse_to_slot( x,y, &ox,&oy);
    toolbelt_hud_mouse_to_slot( x,y, &ox,&oy);

    // select slot internally
}

void left_mouse_up(int x, int y)
{
    //printf("t_item::left_mouse_up \n");

    // detect inventory element
    // if none, remove
    // else
    //  compare to left mouse down
    // swap between/within
}

void right_mouse_down(int x, int y)
{
    //printf("t_item::right_mouse_down \n");

    // lock slot
}

void right_mouse_up(int x, int y)
{
    //printf("t_item::right_mouse_up \n");

    // if slot matches locked slot,
    // remove
}

void null_input_event()
{
    inventory_input_event.type = INVENTORY_INPUT_EVENT_NONE;
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
