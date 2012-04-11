#include "client.hpp"

#if DC_CLIENT

#include <c_lib/draw/transparent.hpp>

void register_inventory_item_draw_list(InventoryProperties* property)
{
    Draw::draw_lists->get(Draw::ITEM_DRAW_LIST)->register_object(property);
}

void unregister_inventory_item_draw_list(InventoryProperties* property)
{
    Draw::draw_lists->get(Draw::ITEM_DRAW_LIST)->unregister_object(property);
}

void InventoryProperties::load(int id, Object_types type)
{
    bool new_icon = (id != this->item_id || type != this->item_type);
    if (this->item_id != EMPTY_SLOT && new_icon)
        unregister_inventory_item_draw_list(this);
    if (type != this->item_type)
    {
        this->sprite_index = get_icon_spritesheet_id(type);
        // MAJO TODO -- HACK
        // hud icon spritesheet is 8x8 but rendered particle spritesheet is 16x16 -- but using same indexes
        // (should be using same sheet)
        if (this->sprite_index >= 8)
            this->sprite_index -= 8;
    }
    this->item_id = id;
    this->item_type = type;
    if (id != EMPTY_SLOT && new_icon)   // set to new
        register_inventory_item_draw_list(this);
    printf("Loaded inventory item %d,%d\n", id,type);
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
    const float spacing = 64.0f;
    data->x = inventory->screen.x + spacing * slot;
    data->y = inventory->screen.y;
    data->z = inventory->screen.z;
}

void Inventory::attach_to_owner()
{
    Agent_state* a = STATE::agent_list->get(this->get_owner());
    if (a == NULL)
    {
        printf("WARNING: Inventroy::attach_to_owner() -- agent %d not found\n", this->get_owner());
        return;
    }
    if (a->status.inventory != NULL)
        printf("WARNING: reassigned agent inventory\n");
    a->status.inventory = this;
}

#endif
