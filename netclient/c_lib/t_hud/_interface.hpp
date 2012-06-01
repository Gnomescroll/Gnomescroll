#pragma once

namespace t_hud
{

// UI State
extern class AgentContainerUI* agent_container;
extern class AgentToolbeltUI* agent_toolbelt;
extern class AgentNaniteUI* nanite_container;
extern class CraftingUI* crafting_container;
extern class StorageBlockUI* storage_block;

void set_container_id(ItemContainerType container_type, int container_id);

extern float mouse_x;
extern float mouse_y;

// Input Handling
typedef struct
{
    int container_id;
    int slot;
    bool nanite_shopping;
    bool craft_output;
} ContainerInputEvent;

void enable_agent_container_hud();
void disable_agent_container_hud();
void enable_crafting_container_hud();
void disable_crafting_container_hud();
void enable_storage_block_hud();
void disable_storage_block_hud();

void close_container(int container_id);

ContainerInputEvent left_mouse_down(int x, int y);
ContainerInputEvent left_mouse_up(int x, int y);
ContainerInputEvent right_mouse_down(int x, int y);
ContainerInputEvent right_mouse_up(int x, int y);
ContainerInputEvent mouse_motion(int x, int y);
ContainerInputEvent scroll_up();
ContainerInputEvent scroll_down();
ContainerInputEvent select_slot(int numkey);
ContainerInputEvent null_input_event();

void set_mouse_position(int x, int y);

// network
void network_container_assignment(ObjectType type, int id);

// Init
void init();
void teardown();

void draw_init();
void draw_teardown();

// render
void draw_hud();

}
