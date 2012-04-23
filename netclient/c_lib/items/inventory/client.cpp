#include "client.hpp"

#if DC_CLIENT

#include <c_lib/hud/inventory.hpp>

void InventoryProperties::load(int id, ObjectType type, int stack_size)
{
    if (type != this->item_type)
    {
        //this->sprite_index = get_icon_spritesheet_id(type);
        // MAJO TODO -- HACK
        // hud icon spritesheet is 8x8 but rendered particle spritesheet is 16x16 -- but using same indexes
        // (should be using same sheet)
        //if (this->sprite_index >= 8)
            //this->sprite_index -= 8 * this->sprite_index/16;
    }
    this->item_id = id;
    this->item_type = type;
    this->stack.count = stack_size;
    this->stack.max = get_max_stack_size(type);
    //printf("Loaded inventory item %d,%d\n", this->item_id, this->item_type);
}

void InventoryProperties::get_sprite_data(struct Draw::SpriteData* data)
{
    data->index = this->sprite_index;
    float x = HudInventory::inventory->x + (this->spacing/4);
    float y = HudInventory::inventory->y + HudInventory::inventory->height - 3*(this->spacing/4); // need to subtract our height
    data->x = x + this->spacing * (slot % this->inventory->width());
    data->y = y - this->spacing * (slot / this->inventory->width());
    data->z = HudInventory::inventory->z + 0.01;
}


/* Inventory */

void Inventory::get_selected_icon_render_data(Draw::SpriteData* data)
{
    if (!this->selected()) return;

    InventoryProperties icon = this->contents.objects[0];
    
    float x = HudInventory::inventory->x;// + (icon.spacing/4);
    float y = HudInventory::inventory->y + HudInventory::inventory->height - icon.spacing; // need to subtract our height
    data->x = x + icon.spacing * (this->selected_slot % this->width());
    data->y = y - icon.spacing * (this->selected_slot / this->width());
    data->z = HudInventory::inventory->z + 0.01 + 0.01;
    data->w = icon.spacing;
    data->h = icon.spacing;
}


Inventory::Inventory(int id)
: BaseInventoryClient(id), selected_slot(-1)
{
}

void attach_inventory_to_owner(Inventory* inventory, int owner)
{
    Agent_state* a = STATE::agent_list->get(owner);
    if (a == NULL)
    {
        printf("WARNING: Inventory::attach_to_owner() -- agent %d not found\n", owner);
        return;
    }
    if (a->status.inventory != NULL)
        printf("WARNING: reassigned agent inventory\n");
    a->status.inventory = inventory;
}

#endif
