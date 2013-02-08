#pragma once

#include <item/container/container_ui.hpp>

namespace HudContainer
{

const int ITEM_ICON_RENDER_SIZE = 48;

// UI State
extern class AgentInventoryUI* inventory;
extern class AgentToolbeltUI* toolbelt;
extern class AgentSynthesizerUI* synthesizer;
extern class CacheUI* cache;
extern class CraftingUI* crafting_bench;
extern class StorageBlockUI* storage_block;
extern class EnergyTanksUI* energy_tanks;
extern class CrusherUI* crusher;
extern class EquipmentUI* equipment;

void set_container_id(ItemContainerType container_type, ItemContainerID container_id);

extern float mouse_x;
extern float mouse_y;
extern bool lm_down;    // left mouse down

// Input Handling
typedef struct
{
    ItemContainerID container_id;
    int slot;
    bool alt_action;
}   ContainerInputEvent;

void enable_agent_inventory_hud();
void disable_agent_inventory_hud();

// parameterize these:
void enable_container_block_hud(ItemContainerID container_id);
void disable_container_block_hud();

void close_container(ItemContainerID container_id);

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
void network_container_assignment(EntityType type, int id);

// Init
void init();
void teardown();

void draw_init();
void draw_teardown();

// render
void draw();
void draw_tracking_pixel(float x, float y);

void draw_durability_meter(float x, float y, int slot_size, unsigned char alpha,
                           const struct ItemContainer::SlotMetadata& metadata);

}   // HudContainer
