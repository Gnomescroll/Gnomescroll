#pragma once

namespace t_hud
{

// UI State
extern class AgentInventoryUI* agent_inventory;
extern class AgentToolbeltUI* agent_toolbelt;
// TODO -- TMP -- replace witha ctual types
extern class AgentNaniteUI* nanite_inventory;
extern class AgentInventoryUI* craft_bench_inventory;

void set_container_id(ItemContainerType container_type, int container_id);

extern float mouse_x;
extern float mouse_y;

// Input Handling
typedef struct
{
    int container_id;
    int slot;
} ContainerInputEvent;

void enable_inventory_hud();
void disable_inventory_hud();

ContainerInputEvent left_mouse_down(int x, int y);
ContainerInputEvent left_mouse_up(int x, int y);
ContainerInputEvent right_mouse_down(int x, int y);
ContainerInputEvent right_mouse_up(int x, int y);
ContainerInputEvent mouse_motion(int x, int y);
ContainerInputEvent null_input_event();

// network
void network_inventory_assignment(ObjectType type, int id);

// Init
void init();
void teardown();

void draw_init();
void draw_teardown();

// render
void draw_hud();

}
