#include "client.hpp"

#if DC_CLIENT

#include <c_lib/draw/transparent.hpp>
#include <c_lib/hud/inventory.hpp>

void register_inventory_item_draw_list(InventoryProperties* property)
{
    Draw::draw_lists->get(Draw::ITEM_DRAW_LIST)->register_object(property);
}

void unregister_inventory_item_draw_list(InventoryProperties* property)
{
    Draw::draw_lists->get(Draw::ITEM_DRAW_LIST)->unregister_object(property);
}

void InventoryProperties::load(int id, Object_types type, int subtype)
{
    bool new_icon = (id != this->item_id || type != this->item_type || subtype != this->item_subtype);
    if (this->item_id != EMPTY_SLOT && new_icon)
        unregister_inventory_item_draw_list(this);
    if (type != this->item_type || subtype != this->item_subtype)
    {
        this->sprite_index = get_icon_spritesheet_id(type, subtype);
        // MAJO TODO -- HACK
        // hud icon spritesheet is 8x8 but rendered particle spritesheet is 16x16 -- but using same indexes
        // (should be using same sheet)
        if (this->sprite_index >= 8)
            this->sprite_index -= 8;
    }
    this->item_id = id;
    this->item_type = type;
    this->item_subtype = subtype;
    if (id != EMPTY_SLOT && new_icon)   // set to new
        register_inventory_item_draw_list(this);
    //printf("Loaded inventory item %d,%d\n", id,type);
}


Inventory::Inventory(int id)
: BaseInventoryClient(id)
{
    // todo -- get the inventory window information elsewhere
    this->screen.x = _xresf/3;
    this->screen.y = _yresf - 64;
    this->screen.z = -0.1f;
}

void InventoryProperties::get_sprite_data(struct Draw::SpriteData* data)
{
    data->index = this->sprite_index;
    //data->x = inventory->screen.x + this->spacing * slot;
    //data->y = inventory->screen.y;
    //data->z = inventory->screen.z;
    float x = HudInventory::inventory->x + (this->spacing/4);
    float y = HudInventory::inventory->y + HudInventory::inventory->height - 3*(this->spacing/4); // need to subtract our height
    data->x = x + this->spacing * (slot % this->inventory->width());
    data->y = y - this->spacing * (slot / this->inventory->width());
    data->z = HudInventory::inventory->z + 0.01;
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
