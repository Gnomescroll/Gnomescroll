#pragma once

namespace t_hud
{

// UI state
extern int active_slot;
extern class UIElement* active_inventory;
extern float mouse_x;
extern float mouse_y;

extern class AgentInventoryUI* agent_inventory;
extern class AgentToolbeltUI* agent_toolbelt;


// TODO -- TMP -- replace witha ctual types
extern class AgentNanite* nanite_inventory;
extern class AgentInventoryUI* craft_bench_inventory;


/*
    Hud UI state
*/

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

void null_input_event();

void network_inventory_assignment(ObjectType type, int id);

/*
Init
*/
void init();
void teardown();

void draw_init();
void draw_teardown();

}
